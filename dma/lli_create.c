#include "dma.h"

/*
 * Destination / Source Transfer Width [P.156]
 * Used in CHx_CTL.SRC_TR_WIDTH / CHx_CTL.DST_TR_WIDTH
 * size is 8 ~ 512 bits
 */
#define AXI4_BURST_SIZE_1_BYTE 0b000
#define AXI4_BURST_SIZE_2_BYTE 0b001
#define AXI4_BURST_SIZE_4_BYTE 0b010
#define AXI4_BURST_SIZE_8_BYTE 0b011
#define AXI4_BURST_SIZE_16_BYTE 0b100
#define AXI4_BURST_SIZE_32_BYTE 0b101
#define AXI4_BURST_SIZE_64_BYTE 0b110

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

static inline uint64_t CTL_concatenate(uint64_t lli_last) {
  uint64_t CHx_CTL =
      ((uint64_t)1 << 63) |                                   // SHADOWREG_OR_LLI_VALID, Shadow Register content/Linked List Item Valid
      ((uint64_t)lli_last << 62) |                            // SHADOWREG_OR_LLI_LAST, Last Shadow Register/Linked List Item. 0 = Not last Shadow Register/LLI 1 = Last Shadow Register/LLI
      ((uint64_t)0 << 58) |                                   // IOC_BlkTfr, Interrupt On completion of Block Transfer
      ((uint64_t)0 << 57) |                                   // DST_STAT_EN, Destination Status Enable, Enable the logic to fetch status from destination peripheral of channel x pointed to by the content of CHx_DSTATAR register and stores it in CHx_DSTAT register.
      ((uint64_t)0 << 56) |                                   // SRC_STAT_EN, Source Status Enable, Enable the logic to fetch status from source peripheral of channel x pointed to by the content of CH1_SSTATAR register and stores it in CHx_SSTAT register.
      ((uint64_t)(destination_max_burst_length - 1) << 48) |  // AWLEN, Destination Max Burst Length
      ((uint64_t)1 << 47) |                                   // AWLEN_EN, Destination Burst Length Enable
      ((uint64_t)(source_max_burst_length - 1) << 39) |       // ARLEN, Source Max Burst Length
      ((uint64_t)1 << 38) |                                   // ARLEN_EN, Source Burst Length Enable
      ((uint64_t)0 << 35) |                                   // AW_PROT, AXI 'aw_prot' signal
      ((uint64_t)0 << 32) |                                   // AR_PROT, AXI 'ar_prot' signal
      ((uint64_t)1 << 30) |                                   // NonPosted_LastWrite_En, Non Posted Last Write Enable, This bit decides whether posted writes can be used throughout the block transfer or not.
      ((uint64_t)0 << 26) |                                   // AW_CACHE, AXI 'aw_cache' signal
      ((uint64_t)0 << 22) |                                   // AR_CACHE, AXI 'ar_cache' signal
      ((uint64_t)2 << 18) |                                   // DST_MSIZE, Destination Burst Transaction Length. 0010=8bytes?
      ((uint64_t)2 << 14) |                                   // SRC_MSIZE, Source Burst Transaction Length. 0010=8bytes?
      ((uint64_t)(destination_transfer_width) << 11) |        // DST_TR_WIDTH, Destination Transfer Width 011=64bits
      ((uint64_t)(source_transfer_width) << 8) |              // SRC_TR_WIDTH, Source Transfer Width 011=64bits
      ((uint64_t)0 << 6) |                                    // DINC, Destination Address Increment,0 = Increment 1 = No Change
      ((uint64_t)0 << 4) |                                    // SINC, Source Address Increment,0 = Increment 1 = No Change
      ((uint64_t)0 << 2) |                                    // DMS, Destination Master Select, 0 = AXI master 1 1 = AXI Master 2
      ((uint64_t)0 << 0);                                     // SMS, Source Master Select, 0 = AXI master 1 1 = AXI Master 2
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
  lli->Reserved1                = 0;
  lli->CHx_BLOCK_TS             = transfer_length_byte / pow(2, source_transfer_width);
  lli->CHx_DAR                  = destination_addr;
  lli->CHx_SAR                  = source_addr;
#ifdef DEBUG_DMA
  printf("\n--------------------------------------------\n");
  printf("Reserved0                   %lx\n", current_lli->Reserved0);
  printf("CHx_LLP_STATUS              %lx\n", current_lli->CHx_LLP_STATUS);
  printf("WB_CHx_DSTAT | WB_CHx_SSTAT %x%x\n", current_lli->WB_CHx_DSTAT, current_lli->WB_CHx_SSTAT);
  printf("CHx_CTL                     %lx\n", current_lli->CHx_CTL);
  printf("CHx_LLP                     %lx\n", current_lli->CHx_LLP);
  printf("Reserved1 | CHx_BLOCK_TS    %x%x\n", current_lli->Reserved1, current_lli->CHx_BLOCK_TS);
  printf("CHx_DAR                     %lx\n", current_lli->CHx_DAR);
  printf("CHx_SAR                     %lx\n", current_lli->CHx_SAR);
  printf("--------------------------------------------\n");
#endif
}

