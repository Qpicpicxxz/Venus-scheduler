#include "dma/common.h"

uint32_t DMAC_get_free_channel(void);
void     DMAC_CHx_cfg_write(uint64_t CHx_CFG, uint32_t free_channel_index);
void     DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
void     DMAC_CHx_enable_channel(uint32_t free_channel_index);
void     DMAC_config(void);
void     DMAC_reset(void);
void     DMAC_interrupt_handler(void);
void     DMAC_CommonReg_interrupt_handler(void);
void     DMAC_CHx_interrupt_handler(uint32_t channel_index);

#define CHANNEL_MASK   ((1 << DMAC_NUMBER_OF_CHANNELS) - 1)
#define BIT_PICK(N, i) ((N >> i) & 1)
#define ALL_1_RESET    0xffffffffffffffff

static inline uint64_t read_burst_64(uint32_t base, uint32_t offset) {
#ifndef SIMULATE_QEMU
  return *(volatile uint64_t*)(base + offset);
#else
  printf("READ_BURST_64: Reading address %p\n", base + offset);
  return 0;
#endif
}
static inline void write_burst_64(uint32_t base, uint32_t offset, uint64_t value) {
#ifndef SIMULATE_QEMU
  volatile uint64_t* localaddr = (volatile uint64_t*)(base + offset);
  *localaddr                   = value;
#else
  printf("WRITE_BURST_64: Writing address %p\n", base + offset);
#endif
}
static inline uint32_t read_burst_32(uint32_t base, uint32_t offset) {
#ifndef SIMULATE_QEMU
  return *(volatile uint32_t*)(base + offset);
#else
  printf("READ_BURST_32: Reading address %p\n", base + offset);
  return 0;
#endif
}
static inline void write_burst_32(uint32_t base, uint32_t offset, uint32_t value) {
#ifndef SIMULATE_QEMU
  volatile uint32_t* localaddr = (volatile uint32_t*)(base + offset);
  *localaddr                   = value;
#else
  printf("WRITE_BURST_32: Writing address %p\n", base + offset);
#endif
}
static inline uint32_t read_burst_16(uint32_t base, uint32_t offset) {
#ifndef SIMULATE_QEMU
  return *(volatile uint16_t*)(base + offset);
#else
  printf("READ_BURST_16: Reading address %p\n", base + offset);
  return 0;
#endif
}
static inline void write_burst_16(uint32_t base, uint32_t offset, uint16_t value) {
#ifndef SIMULATE_QEMU
  volatile uint16_t* localaddr = (volatile uint16_t*)(base + offset);
  *localaddr                   = value;
#else
  printf("WRITE_BURST_16: Writing address %p\n", base + offset);
#endif
}
