#include "common.h"
#include "dma.h"
#include "hw/axi.h"
#include "hw/config.h"

/* CHx_CTL in bits 62: SHADOWREG_OR_LLI_LAST [P.150] */
#define NOT_LAST_SHADOW_REGISTER 0
#define LAST_SHADOW_REGISTER     1

extern void vcs_stop(void);

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
/* mem.S */
extern uint32_t TASKA_START;
extern uint32_t TASKA_END;
extern uint32_t TASKA_LEN;
extern uint32_t TASK2_START;
extern uint32_t TASK2_END;
extern uint32_t TASK2_LEN;

uint64_t destination_max_burst_length = 256;
uint64_t source_max_burst_length      = 256;
uint32_t destination_transfer_width   = AXI4_BURST_SIZE_8_BYTE;
uint32_t source_transfer_width        = AXI4_BURST_SIZE_8_BYTE; /* SRC_TR_WIDTH */

static lli_t* head_lli;

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
uint32_t cluster;
uint32_t block;

/* From the SoC's perspective:
 *  Nth cluster, Kth block (0<=N<=7, 0<=K<=15)
 *  I-SPM = VENUS_CLUSTER_ADDR + CLUSTER_N_OFFSET + BLOCK_K_OFFSET + BLOCK_ISPM_OFFSET
 */

// 2.找到一个空闲的venus block
//     去读取 0x2030_0000 + K*0x40_0000 + N*0x400_0000 地址上的Control Registers 中的 VenusBlock_CfgReg寄存器，判断bit[0]、bit[1]中的soft reset，和waiting for write back是否均为0,
// 若均为0表示该block空闲。若soft reset为1表示该block正忙，若soft reset为0且waiting for write back为1，表示该block不在运行但正在等待数据取回完成。
//     (采用其他方式也可)
// 3.等待DMAC传输完成中断到来
void dma_scheduler_test1() {
  // simulate to find a idle block
  // TODO: 如果找不到idle的block就卡住...
  for (int i = 0; i < NUM_CLUSTERS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      uint32_t block_cfg = READ_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(i) + BLOCK_OFFSET(j), BLOCK_CTRLREGS_OFFSET);
      if (!BIT_PICK(block_cfg, 0)) {
        cluster = i;
        block   = j;
        // found an idle block, break out of two for-loops
        goto next_step;
      }
    }
  }
next_step:
  lli_t* current_lli                = malloc_LLI();
  uint64_t destination_addr         = (uint64_t)(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(cluster) + BLOCK_OFFSET(block) + BLOCK_ISPM_OFFSET);
  uint64_t source_addr              = (uint64_t)TASKA_START;
  uint32_t transfer_length_byte     = TASKA_LEN;
  uint32_t total_chunk              = ((transfer_length_byte - 1) / DMA_MAX_TRANSFER_LENGTH) + 1;
  uint64_t current_source_addr      = source_addr;
  uint64_t current_destination_addr = destination_addr;
  uint64_t left_transfer_length     = transfer_length_byte;
  head_lli                          = current_lli;

  /* 0x00000000 .2byte 0x0000
   *             2 * 8  4 * 4  16bit
   * 0x00000002
   */
  for (int i = 0; i < total_chunk; i++) {
    if (i == total_chunk - 1) {
      lli_setup(
          current_destination_addr,
          current_source_addr,
          left_transfer_length,
          current_lli,
          current_lli,
          LAST_SHADOW_REGISTER);
    } else {
      lli_t* next_lli = malloc_LLI();
      lli_setup(
          current_destination_addr,
          current_source_addr,
          DMA_MAX_TRANSFER_LENGTH,
          current_lli,
          next_lli,
          NOT_LAST_SHADOW_REGISTER);
      current_source_addr      = current_source_addr + DMA_MAX_TRANSFER_LENGTH;
      current_destination_addr = current_destination_addr + DMA_MAX_TRANSFER_LENGTH;
      left_transfer_length     = left_transfer_length - DMA_MAX_TRANSFER_LENGTH;
      current_lli              = next_lli;
    }
  }
  printf("byte = %d, chunk = %d\n", transfer_length_byte, total_chunk);
  uint32_t free_channel_index = DMAC_get_free_channel();
  /* configure and active DMA */
  CFG_config(free_channel_index);
  DMAC_CHx_specify_first_lli(head_lli, free_channel_index);
  DMAC_CHx_enable_channel(free_channel_index);
}
void dma_scheduler_test2() {
  // simulate to find a idle block
  // TODO: 如果找不到idle的block就卡住...
  for (int i = 0; i < NUM_CLUSTERS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      uint32_t block_cfg = READ_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(i) + BLOCK_OFFSET(j), BLOCK_CTRLREGS_OFFSET);
      if (!BIT_PICK(block_cfg, 0)) {
        cluster = i;
        block   = j;
        // found an idle block, break out of two for-loops
        goto next_step;
      }
    }
  }
next_step:
  lli_t* current_lli                = malloc_LLI();
  uint64_t destination_addr         = (uint64_t)(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(cluster) + BLOCK_OFFSET(block) + BLOCK_ISPM_OFFSET);
  uint64_t source_addr              = (uint64_t)TASK2_START;
  uint32_t transfer_length_byte     = TASK2_LEN;
  uint32_t total_chunk              = ((transfer_length_byte - 1) / DMA_MAX_TRANSFER_LENGTH) + 1;
  uint64_t current_source_addr      = source_addr;
  uint64_t current_destination_addr = destination_addr;
  uint64_t left_transfer_length     = transfer_length_byte;
  head_lli                          = current_lli;

  /* 0x00000000 .2byte 0x0000
   *             2 * 8  4 * 4  16bit
   * 0x00000002
   */
  for (int i = 0; i < total_chunk; i++) {
    if (i == total_chunk - 1) {
      lli_setup(
          current_destination_addr,
          current_source_addr,
          left_transfer_length,
          current_lli,
          current_lli,
          LAST_SHADOW_REGISTER);
    } else {
      lli_t* next_lli = malloc_LLI();
      lli_setup(
          current_destination_addr,
          current_source_addr,
          DMA_MAX_TRANSFER_LENGTH,
          current_lli,
          next_lli,
          NOT_LAST_SHADOW_REGISTER);
      current_source_addr      = current_source_addr + DMA_MAX_TRANSFER_LENGTH;
      current_destination_addr = current_destination_addr + DMA_MAX_TRANSFER_LENGTH;
      left_transfer_length     = left_transfer_length - DMA_MAX_TRANSFER_LENGTH;
      current_lli              = next_lli;
    }
  }
  printf("byte = %d, chunk = %d\n", transfer_length_byte, total_chunk);
  uint32_t free_channel_index = DMAC_get_free_channel();
  /* configure and active DMA */
  CFG_config(free_channel_index);
  DMAC_CHx_specify_first_lli(head_lli, free_channel_index);
  DMAC_CHx_enable_channel(free_channel_index);
}

void task_test(void) {
  dma_scheduler_test1();
  dma_scheduler_test2();
}

//     3.1 中断到来后，向所选定的venus block 的 Control Registers 中的 VenusBlock_CfgReg寄存器的bit[0]写入1，解开soft reset，解开后venus block开始执行task
//         VENUS_SCHEDULER_BFM_WRITE_BURST4( VENUS_CLUSTER_ADDR + CLUSTER_0_OFFSET + BLOCK_0_OFFSET + BLOCK_CTRLREGS_OFFSET,32'h0,32'h0000_0001,`ENABLE_MESSAGE);
//     3.2 清除DMAC 对应CHANNEL的中断
// TODO: 我能知道是哪个block的中断完成了吗？
void dma_transmit_done_callback(uint32_t channel_index) {
  if (channel_index == 0)
    WRITE_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(0) + BLOCK_OFFSET(0), BLOCK_CTRLREGS_OFFSET, EN_SOFT_RST);
  else if (channel_index == 1)
    WRITE_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(0) + BLOCK_OFFSET(1), BLOCK_CTRLREGS_OFFSET, EN_SOFT_RST);
}

