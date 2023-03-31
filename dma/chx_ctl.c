#include "dma/common.h"
#include "dma/ctl.h"

/* CHx_CTL: Control the DMA transfer [P.148] */
uint64_t CTL_config(uint64_t lli_last) {
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
