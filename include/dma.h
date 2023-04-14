#ifndef __DMA_H_
#define __DMA_H_

#include "assert.h"
#include "hw/addressmap.h"
#include "hw/dma.h"
#include "lli.h"
#include "types.h"
#include "ulib.h"

#define CHANNEL_MASK   ((1 << DMAC_NUMBER_OF_CHANNELS) - 1)
#define BIT_PICK(N, i) ((N >> i) & 1)
#define ALL_1_RESET    0xffffffffffffffff

/* dma tools */
extern lli_t* malloc_LLI(void);
extern void free_LLI(lli_t* ptr);

extern uint64_t destination_max_burst_length;
extern uint64_t source_max_burst_length;
extern uint32_t destination_transfer_width;
extern uint32_t source_transfer_width;

/* printf */
extern int printf(const char* s, ...);  // only understands %d, %x, %p, %s

#endif /* __DMA_H_ */
