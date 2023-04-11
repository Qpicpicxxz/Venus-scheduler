#ifndef __DMA_LLI_H__
#define __DMA_LLI_H__

#include "types.h"

typedef struct lli {
  uint64_t Reserved0;
  uint64_t CHx_LLP_STATUS;
  uint32_t WB_CHx_DSTAT;
  uint32_t WB_CHx_SSTAT;
  uint64_t CHx_CTL;
  uint64_t CHx_LLP;
  uint32_t Reserved1;
  uint32_t CHx_BLOCK_TS;
  uint64_t CHx_DAR;
  uint64_t CHx_SAR;
} lli_t;

/* 
 * +-----  DW_axi_dmac Linked List Item (Descriptor)  -----+
 * | 0x3C | 31                 Reserved                  0 |
 * | 0x38 | 31                 Reserved                  0 |
 * | 0x34 | 31          CHx_LLP_STATUS [63:32]           0 |
 * | 0x30 | 31          CHx_LLP_STATUS [31: 0]           0 |
 * | 0x2C | 31         Write back for CHx_DSTAT          0 |
 * | 0x28 | 31         Write back for CHx_SSTAT          0 |
 * | 0x24 | 31              CHx_CTL [63:32]              0 |+
 * | 0x20 | 31              CHx_CTL [31: 0]              0 |+
 * | 0x1C | 31              CHx_LLP [63:32]              0 |+
 * | 0x18 | 31      CHx_LLP [31: 5]     6 | 5  Reserved  0 |+
 * | 0x14 | 31                 Reserved                  0 |
 * | 0x10 | 31            CHx_BLOCK_TS [31:0]            0 |+
 * | 0x0C | 31              CHx_DAR [63:32]              0 |+
 * | 0x08 | 31              CHx_DAR [31: 0]              0 |+
 * | 0x04 | 31              CHx_SAR [63:32]              0 |+
 * | 0x00 | 31              CHx_SAR [31: 0]              0 |+
 * +-------------------------------------------------------+
*/

#endif /* __DMA_LLI_H__ */
