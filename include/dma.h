#ifndef __DMA_H__
#define __DMA_H__

#include "task.h"
#include "lli.h"

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

#define DMA_MAX_TRANSFER_LENGTH (2048 * 8)
/* CHx_CTL in bits 62: SHADOWREG_OR_LLI_LAST [P.150] */
#define NOT_LAST_SHADOW_REGISTER 0
#define LAST_SHADOW_REGISTER 1

/* read/write physical memory value */
extern uint64_t read_burst_64(uint32_t addr, uint32_t offset);
extern void write_burst_64(uint32_t addr, uint32_t offset, uint64_t value);
extern uint32_t read_burst_32(uint32_t addr, uint32_t offset);
extern void write_burst_32(uint32_t addr, uint32_t offset, uint32_t value);

/* linked list item generate */
extern void lli_setup(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last);

#endif /* __DMA_H__ */
