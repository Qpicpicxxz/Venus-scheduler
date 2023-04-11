#include "dma/common.h"

uint32_t DMAC_get_free_channel(void);
void DMAC_CHx_cfg_write(uint64_t CHx_CFG, uint32_t free_channel_index);
void DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
void DMAC_CHx_enable_channel(uint32_t free_channel_index);
void DMAC_config(void);
void DMAC_reset(void);
void DMAC_interrupt_handler(void);
void DMAC_CommonReg_interrupt_handler(void);
void DMAC_CHx_interrupt_handler(uint32_t channel_index);

#define CHANNEL_MASK   ((1 << DMAC_NUMBER_OF_CHANNELS) - 1)
#define BIT_PICK(N, i) ((N >> i) & 1)
#define ALL_1_RESET    0xffffffffffffffff

#ifndef SIMULATE_QEMU
#define READ_BURST_64(base, offset)         (*(volatile uint64_t*)((base) + (offset)))
#define WRITE_BURST_64(base, offset, value) (*(volatile uint64_t*)((base) + (offset)) = (value))
#define READ_BURST_32(base, offset)         (*(volatile uint32_t*)((base) + (offset)))
#define WRITE_BURST_32(base, offset, value) (*(volatile uint32_t*)((base) + (offset)) = (value))
#define READ_BURST_16(base, offset)         (*(volatile uint16_t*)((base) + (offset)))
#define WRITE_BURST_16(base, offset, value) (*(volatile uint16_t*)((base) + (offset)) = (value))
#else
#define READ_BURST_64(base, offset) \
  printf("READ_BURST_64: Reading address %p\n", (base) + (offset));
#define WRITE_BURST_64(base, offset, value) \
  printf("WRITE_BURST_64: Writing %p to address %p\n", (base) + (offset), (value));
#define READ_BURST_32(base, offset) \
  printf("READ_BURST_32: Reading address %p\n", (base) + (offset), (value));
#define WRITE_BURST_32(base, offset, value) \
  printf("WRITE_BURST_32: Writing %p to address %p\n", (base) + (offset));
#define READ_BURST_16(base, offset) \
  printf("READ_BURST_16: Reading address %p\n", (base) + (offset));
#define WRITE_BURST_16(base, offset, value) \
  printf("WRITE_BURST_16: Writing %p to address %p\n", (base) + (offset), (value));
#endif