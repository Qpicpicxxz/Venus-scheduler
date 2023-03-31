#ifndef __DMA_COMMON_H__
#define __DMA_COMMON_H__

#include "dma/axi.h"
#include "dma/lli.h"
#include "task.h"


#define DMA_MAX_TRANSFER_LENGTH (2048 * 8)

/* CHx_CTL in bits 62: SHADOWREG_OR_LLI_LAST [P.150] */
#define NOT_LAST_SHADOW_REGISTER 0
#define LAST_SHADOW_REGISTER     1

/* read/write physical memory value */
extern uint64_t read_burst_64(uint32_t addr, uint32_t offset);
extern void write_burst_64(uint32_t addr, uint32_t offset, uint64_t value);
extern uint32_t read_burst_32(uint32_t addr, uint32_t offset);
extern void write_burst_32(uint32_t addr, uint32_t offset, uint32_t value);

/* linked list item generate */
extern void lli_setup(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last);

#endif /* __DMA_COMMON_H__ */
