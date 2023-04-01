#include "dma/common.h"

/* lli_create.c */
extern void lli_setup(uint64_t destination_addr, uint64_t source_addr, uint32_t transfer_length_byte, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last);
/* chx_cfg.c */
extern void cfg_config(uint32_t free_channel_index);
/* phy_interface.c */
extern uint32_t DMAC_get_free_channel(void);
extern void     DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
extern void     DMAC_CHx_enable_channel(uint32_t free_channel_index);

static lli_t*   head_lli;
static uint64_t destination_addr;
static uint64_t source_addr;
static uint32_t transfer_length_byte;

static inline void lli_link(void) {
  lli_t*   current_lli          = head_lli;
  uint32_t total_chunk          = (transfer_length_byte / DMA_MAX_TRANSFER_LENGTH) + 1;
  uint64_t current_source_addr  = source_addr;
  uint64_t left_transfer_length = transfer_length_byte;
  for (int i = 0; i < total_chunk; i++) {
    // if its the last chunk to be transmitted
    if (i == total_chunk - 1) {
      lli_setup(destination_addr,
                current_source_addr,
                left_transfer_length,
                current_lli,
                current_lli,
                LAST_SHADOW_REGISTER);
    } else {
      lli_t* next_lli = malloc_LLI();
      lli_setup(destination_addr,
                current_source_addr,
                DMA_MAX_TRANSFER_LENGTH,
                current_lli,
                next_lli,
                NOT_LAST_SHADOW_REGISTER);
      current_source_addr  = current_source_addr + 4 * (DMA_MAX_TRANSFER_LENGTH);
      left_transfer_length = left_transfer_length - DMA_MAX_TRANSFER_LENGTH;
      current_lli          = next_lli;
    }
  }
}

void dma_transfer(uint32_t dst, uint32_t src, uint32_t len) {
  head_lli             = malloc_LLI();
  destination_addr     = (uint64_t)dst;
  source_addr          = (uint64_t)src;
  transfer_length_byte = len;
  lli_link();
  uint32_t free_channel_index = DMAC_get_free_channel();
  cfg_config(free_channel_index);
  DMAC_CHx_specify_first_lli(head_lli, free_channel_index);
  DMAC_CHx_enable_channel(free_channel_index);
}

void dma_test(void) {
  uint32_t i_spm_addr = 0x99999999;
  uint32_t task_addr  = 0x33333333;
  uint32_t task_len   = 17000;
  dma_transfer(i_spm_addr, task_addr, task_len);
}

void dma_result(uint32_t data_dst, uint32_t data_addr, uint32_t data_len) {
  // delay(5000);
}

