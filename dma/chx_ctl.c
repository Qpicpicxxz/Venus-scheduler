#include "dma/common.h"
#include "dma/ctl.h"

/* CHx_CTL: Control the DMA transfer [P.148] */
uint64_t CTL_config(uint64_t lli_last) {
  uint64_t CHx_CTL =
      SHADOWREG_OR_LLI_VALID(1) |                 // [SHADOWREG_OR_LLI_VALID][63] Shadow Register content/Linked List Item Valid
      SHADOWREG_OR_LLI_LAST(lli_last) |           // [SHADOWREG_OR_LLI_LAST][62] Last Shadow Register/Linked List Item. 0 = Not last Shadow Register/LLI 1 = Last Shadow Register/LLI
      RESERVED_61_59 |                            // [61:59] Reserved and read as zero
      IOC_BlkTfr(0) |                             // [IOC_BlkTfr][58] Interrupt On completion of Block Transfer
      DST_STAT_EN(0) |                            // [DST_STAT_EN][57] Destination Status Enable, Enable the logic to fetch status from destination peripheral of channel x pointed to by the content of CHx_DSTATAR register and stores it in CHx_DSTAT register.
      SRC_STAT_EN(0) |                            // [SRC_STAT_EN][56] Source Status Enable, Enable the logic to fetch status from source peripheral of channel x pointed to by the content of CH1_SSTATAR register and stores it in CHx_SSTAT register.
      AWLEN(destination_max_burst_length - 1) |   // [AWLEN][55:48] Destination Max Burst Length
      AWLEN_EN(1) |                               // [AWLEN_EN][47] Destination Burst Length Enable
      ARLEN(source_max_burst_length - 1) |        // [ARLEN][46:39] Source Max Burst Length
      ARLEN_EN(1) |                               // [ARLEN_EN][38] Source Burst Length Enable
      AW_PROT(0) |                                // [AW_PROT][37:35] AXI 'aw_prot' signal
      AR_PROT(0) |                                // [AR_PROT][34:32] AXI 'ar_prot' signal
      RESERVED_31 |                               // [31]Reserved and read as zero
      NonPosted_LastWrite_En(1) |                 // [NonPosted_LastWrite_En][30] Non Posted Last Write Enable, This bit decides whether posted writes can be used throughout the block transfer or not.
      AW_CACHE(0) |                               // [AW_CACHE][29:26] AXI 'aw_cache' signal
      AR_CACHE(0) |                               // [AR_CACHE][25:22] AXI 'ar_cache' signal
      DST_MSIZE(2) |                              // [DST_MSIZE][21:18] Destination Burst Transaction Length.  0010=8bytes?
      SRC_MSIZE(2) |                              // [SRC_MSIZE][17:14] Source Burst Transaction Length.  0010=8bytes?
      DST_TR_WIDTH(destination_transfer_width) |  // [DST_TR_WIDTH][13:11] Destination Transfer Width  011=64bits
      SRC_TR_WIDTH(source_transfer_width) |       // [SRC_TR_WIDTH][10:8] Source Transfer Width  011=64bits
      RESERVED_7 |                                // [7] Reserved and read as zero
      DINC(0) |                                   // [DINC][6] Destination Address Increment,0 = Increment 1 = No Change
      RESERVED_5 |                                // [5] Reserved and read as zero
      SINC(0) |                                   // [SINC][4] Source Address Increment,0 = Increment 1 = No Change
      RESERVED_3 |                                // [3] Reserved and read as zero
      DMS(0) |                                    // [DMS][2] Destination Master Select, 0 = AXI master 1 1 = AXI Master 2
      RESERVED_1 |                                // [1] Reserved and read as zero
      SMS(0);                                     // [SMS][0] Source Master Select, 0 = AXI master 1 1 = AXI Master 2
  return CHx_CTL;
}
