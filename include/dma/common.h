#ifndef __DMA_COMMON_H__
#define __DMA_COMMON_H__

#include "assert.h"
#include "common.h"
#include "dma/lli.h"
#include "dma/offset.h"
#include "font.h"
#include "hw/addressmap.h"
#include "types.h"

#define DMA_MAX_TRANSFER_LENGTH (2048 * 8)
#define DMAC_NUMBER_OF_CHANNELS 8

/* dma tools */
extern lli_t* malloc_LLI(void);
extern void free_LLI(lli_t* ptr);

extern uint64_t destination_max_burst_length;
extern uint64_t source_max_burst_length;
extern uint32_t destination_transfer_width;
extern uint32_t source_transfer_width;

/* printf */
extern int printf(const char* s, ...);  // only understands %d, %x, %p, %s

#endif /* __DMA_COMMON_H__ */

