#include "dma/common.h"
#include "dma/ctl.h"

static uint64_t destination_max_burst_length = 256;
static uint64_t source_max_burst_length      = 256;
static uint32_t destination_transfer_width   = AXI4_BURST_SIZE_8_BYTE;
static uint32_t source_transfer_width        = AXI4_BURST_SIZE_8_BYTE; /* SRC_TR_WIDTH */

static inline uint32_t pow(uint32_t base, uint32_t power) {
  uint32_t result = 1;
  for (int i = 0; i < power; i++) {
    result *= base;
  }
  return result;
}

/* CHx_CTL: Control the DMA transfer [P.148] */
static inline uint64_t CTL_concatenate(uint64_t lli_last) {
  uint64_t CHx_CTL =
      SHADOWREG_OR_LLI_VALID(1) |                 // [SHADOWREG_OR_LLI_VALID] Shadow Register content/Linked List Item Valid
      SHADOWREG_OR_LLI_LAST(lli_last) |           // [SHADOWREG_OR_LLI_LAST] Last Shadow Register/Linked List Item. 0 = Not last Shadow Register/LLI 1 = Last Shadow Register/LLI
      RESERVED_61_59 |                            // Reserved and read as zero [61:59]
      IOC_BlkTfr(0) |                             // [IOC_BlkTfr] Interrupt On completion of Block Transfer
      DST_STAT_EN(0) |                            // [DST_STAT_EN] Destination Status Enable, Enable the logic to fetch status from destination peripheral of channel x pointed to by the content of CHx_DSTATAR register and stores it in CHx_DSTAT register.
      SRC_STAT_EN(0) |                            // [SRC_STAT_EN] Source Status Enable, Enable the logic to fetch status from source peripheral of channel x pointed to by the content of CH1_SSTATAR register and stores it in CHx_SSTAT register.
      AWLEN(destination_max_burst_length - 1) |   // [AWLEN] Destination Max Burst Length [55:48]
      AWLEN_EN(1) |                               // [AWLEN_EN] Destination Burst Length Enable
      ARLEN(source_max_burst_length - 1) |        // [ARLEN] Source Max Burst Length [46:39]
      ARLEN_EN(1) |                               // [ARLEN_EN] Source Burst Length Enable
      AW_PROT(0) |                                // [AW_PROT] AXI 'aw_prot' signal [37:35]
      AR_PROT(0) |                                // [AR_PROT] AXI 'ar_prot' signal [34:32]
      RESERVED_31 |                               // Reserved and read as zero [31]
      NonPosted_LastWrite_En(1) |                 // [NonPosted_LastWrite_En] Non Posted Last Write Enable, This bit decides whether posted writes can be used throughout the block transfer or not.
      AW_CACHE(0) |                               // [AW_CACHE] AXI 'aw_cache' signal [29:26]
      AR_CACHE(0) |                               // [AR_CACHE] AXI 'ar_cache' signal [25:22]
      DST_MSIZE(2) |                              // [DST_MSIZE] Destination Burst Transaction Length. [21:18] 0010=8bytes?
      SRC_MSIZE(2) |                              // [SRC_MSIZE] Source Burst Transaction Length. [17:14] 0010=8bytes?
      DST_TR_WIDTH(destination_transfer_width) |  // [DST_TR_WIDTH] Destination Transfer Width [13:11] 011=64bits
      SRC_TR_WIDTH(source_transfer_width) |       // [SRC_TR_WIDTH] Source Transfer Width [10:8] 011=64bits
      RESERVED_7 |                                // Reserved and read as zero [7]
      DINC(0) |                                   // [DINC] Destination Address Increment,0 = Increment 1 = No Change
      RESERVED_5 |                                // Reserved and read as zero [5]
      SINC(0) |                                   // [SINC] Source Address Increment,0 = Increment 1 = No Change
      RESERVED_3 |                                // Reserved and read as zero [3]
      DMS(0) |                                    // [DMS] Destination Master Select, 0 = AXI master 1 1 = AXI Master 2
      RESERVED_1 |                                // Reserved and read as zero [1]
      SMS(0);                                     // [SMS] Source Master Select, 0 = AXI master 1 1 = AXI Master 2
  return CHx_CTL;
}

void lli_setup(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last) {
  uint64_t destination_addr     = (uint64_t)i_spm_addr;
  uint64_t source_addr          = (uint64_t)task_addr;
  uint32_t transfer_length_byte = task_len; /* always <= 2048 * 8 bytes */
  lli_t* lli                    = current_lli;
  uint32_t next_item_pointer    = (uint32_t)next_lli;
  lli->Reserved0                = 0;
  lli->CHx_LLP_STATUS           = 0;
  lli->WB_CHx_DSTAT             = 0;
  lli->WB_CHx_SSTAT             = 0;
  lli->CHx_CTL                  = CTL_concatenate(lli_last);
  lli->CHx_LLP                  = (uint64_t)next_item_pointer;
  lli->CHx_BLOCK_TS = transfer_length_byte / pow(2, source_transfer_width);
  lli->CHx_DAR      = destination_addr;
  lli->CHx_SAR = source_addr;
#ifdef DEBUG_DMA
  printf("\n--------------------------------------------\n");
  printf("Reserved0                   %x%x\n", (uint32_t)(current_lli->Reserved0 >> 32), (uint32_t)(current_lli->Reserved0));
  printf("CHx_LLP_STATUS              %x%x\n", (uint32_t)(current_lli->CHx_LLP_STATUS >> 32), (uint32_t)(current_lli->CHx_LLP_STATUS));
  printf("WB_CHx_DSTAT | WB_CHx_SSTAT %x%x\n", current_lli->WB_CHx_DSTAT, current_lli->WB_CHx_SSTAT);
  printf("CHx_CTL                     %x%x\n", (uint32_t)(current_lli->CHx_CTL >> 32), (uint32_t)(current_lli->CHx_CTL));
  printf("CHx_LLP                     %x%x\n", (uint32_t)(current_lli->CHx_LLP >> 32), (uint32_t)(current_lli->CHx_LLP));
  printf("Reserved1 | CHx_BLOCK_TS    %x%x\n", current_lli->Reserved1, current_lli->CHx_BLOCK_TS);
  printf("CHx_DAR                     %x%x\n", (uint32_t)(current_lli->CHx_DAR >> 32), (uint32_t)(current_lli->CHx_DAR));
  printf("CHx_SAR                     %x%x\n", (uint32_t)(current_lli->CHx_SAR >> 32), (uint32_t)(current_lli->CHx_SAR));
  printf("--------------------------------------------\n");
#endif
}

