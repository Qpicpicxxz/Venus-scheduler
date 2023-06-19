#ifndef __LLI_H__
#define __LLI_H__

#include "types.h"

#define CHx_LLI_SIZE 0x400                              // support up to 16 LLI in each transmission
#define LLI_CH(n)    (0xc0000000 + (n * CHx_LLI_SIZE))  // each channel's LLI addressmap
#define LLI_SIZE     0x40                               // LLI occupy 64-byte memory space

/*
 * RISC-V defalut little-endian
 *   0x00 ------------------------- 0x08
 * 0th byte .................... 63th byte
 */
typedef struct lli {
  uint64_t CHx_SAR;  // 0x00
  uint64_t CHx_DAR;  // 0x08
  uint32_t CHx_BLOCK_TS;
  uint32_t Reserved1;
  uint64_t CHx_LLP;
  uint64_t CHx_CTL;
  uint32_t WB_CHx_SSTAT;
  uint32_t WB_CHx_DSTAT;
  uint64_t CHx_LLP_STATUS;
  uint64_t Reserved0;
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

#endif /* __LLI_H__ */

