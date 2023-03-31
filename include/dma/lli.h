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

#endif /* __DMA_LLI_H__ */
