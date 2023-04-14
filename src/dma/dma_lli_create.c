#include "common.h"
#include "dma.h"
#include "hw/axi.h"

/* CHx_CTL in bits 62: SHADOWREG_OR_LLI_LAST [P.150] */
#define NOT_LAST_SHADOW_REGISTER 0
#define LAST_SHADOW_REGISTER     1

/* dma_config.c */
extern void DMAC_config(void);
extern void DMAC_reset(void);
extern uint64_t CTL_config(uint64_t lli_last);
extern void CFG_config(uint32_t free_channel_index);
/* dma_irq.c */
extern uint32_t DMAC_get_free_channel(void);
extern void DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
extern void DMAC_CHx_enable_channel(uint32_t free_channel_index);
/* start.S */
extern void vcs_stop(void);

uint64_t destination_max_burst_length = 256;
uint64_t source_max_burst_length      = 256;
uint32_t destination_transfer_width   = AXI4_BURST_SIZE_8_BYTE;
uint32_t source_transfer_width        = AXI4_BURST_SIZE_8_BYTE; /* SRC_TR_WIDTH */

void dma_init(void) {
  DMAC_reset();
  DMAC_config();
}

static inline uint32_t pow(uint32_t base, uint32_t power) {
  uint32_t result = 1;
  for (int i = 0; i < power; i++) {
    result *= base;
  }
  return result;
}

void lli_setup(uint64_t destination_addr, uint64_t source_addr, uint32_t transfer_length_byte, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last) {
  lli_t* lli                 = current_lli;
  uint32_t next_item_pointer = (uint32_t)next_lli;
  lli->Reserved0             = 0;
  lli->CHx_LLP_STATUS        = 0;
  lli->WB_CHx_DSTAT          = 0;
  lli->WB_CHx_SSTAT          = 0;
  lli->CHx_CTL               = CTL_config(lli_last);
  lli->CHx_LLP               = (uint64_t)next_item_pointer;
  lli->CHx_BLOCK_TS          = transfer_length_byte / pow(2, source_transfer_width);
  lli->CHx_DAR               = destination_addr;
  lli->CHx_SAR               = source_addr;
}

int data_packet_1[20] = {0, 1, 2, 3, 4, 5};  // 20 * 4 = 80 byte
int data_packet_2[10] = {1, 2, 3};           // 10 * 4 = 40 byte

/* From the SoC's perspective:
 *  Nth cluster, Kth block (0<=N<=7, 0<=K<=15)
 *  I-SPM = VENUS_CLUSTER_ADDR + CLUSTER_N_OFFSET + BLOCK_K_OFFSET + BLOCK_ISPM_OFFSET
 */

void dma_scheduler_test(void) {
  lli_t* lli_p1          = malloc_LLI();
  lli_t* lli_p2          = malloc_LLI();
  uint32_t source_addr_1 = (uint32_t)data_packet_1;
  uint32_t source_addr_2 = (uint32_t)data_packet_2;
  // told dma to transfer data into cluster 0 block 1 I-spm and cluster 2 block 3 I-spm
  lli_setup(
      VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(0) + BLOCK_OFFSET(1) + BLOCK_ISPM_OFFSET,
      (uint64_t)source_addr_1,
      sizeof(data_packet_1),
      lli_p1,
      lli_p2,
      NOT_LAST_SHADOW_REGISTER);
  lli_setup(
      VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(2) + BLOCK_OFFSET(3) + BLOCK_ISPM_OFFSET,
      (uint64_t)source_addr_2,
      sizeof(data_packet_2),
      lli_p2,
      lli_p2,
      LAST_SHADOW_REGISTER);
  uint32_t free_channel_index = DMAC_get_free_channel();
  /* configure and active DMA */
  CFG_config(free_channel_index);
  DMAC_CHx_specify_first_lli(lli_p1, free_channel_index);
  DMAC_CHx_enable_channel(free_channel_index);
}

void dma_transmit_done_callback(uint32_t channel_index) {
  // DMA-lli 简单如上测试没问题 2023/3/14 - 20:57
  print_memory((void*)(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(0) + BLOCK_OFFSET(1) + BLOCK_ISPM_OFFSET), sizeof(data_packet_1));
  print_memory((void*)(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(2) + BLOCK_OFFSET(3) + BLOCK_ISPM_OFFSET), sizeof(data_packet_2));
  vcs_stop();
}
