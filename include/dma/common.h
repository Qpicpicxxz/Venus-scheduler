#ifndef __DMA_COMMON_H__
#define __DMA_COMMON_H__

#include "assert.h"
#include "dma/lli.h"
#include "font.h"
#include "types.h"
#include <stdarg.h>
#include <stddef.h>

#define DMA_MAX_TRANSFER_LENGTH (2048 * 8)

/* CHx_CTL in bits 62: SHADOWREG_OR_LLI_LAST [P.150] */
#define NOT_LAST_SHADOW_REGISTER 0
#define LAST_SHADOW_REGISTER     1

#define DMAC_NUMBER_OF_CHANNELS 8

/* dma tools */
extern lli_t* malloc_LLI(void);
extern void   free_LLI(lli_t* ptr);

extern uint64_t destination_max_burst_length;
extern uint64_t source_max_burst_length;
extern uint32_t destination_transfer_width;
extern uint32_t source_transfer_width;

/* printf */
extern int printf(const char* s, ...);  // only understands %d, %x, %p, %s

#endif /* __DMA_COMMON_H__ */
