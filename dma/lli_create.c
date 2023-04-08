#include "dma/axi.h"
#include "dma/common.h"


/* chx_ctl.c */
extern uint64_t CTL_config(uint64_t lli_last);

uint64_t destination_max_burst_length = 256;
uint64_t source_max_burst_length      = 256;
uint32_t destination_transfer_width   = AXI4_BURST_SIZE_8_BYTE;
uint32_t source_transfer_width        = AXI4_BURST_SIZE_8_BYTE; /* SRC_TR_WIDTH */

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

