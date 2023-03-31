#include "dma/common.h"
#include "dma/addressmap.h"
#include "dma/offset.h"

uint32_t get_free_channel(void);
void     specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
void     enable_channel(uint32_t free_channel_index);

#define CHANNEL_MASK   ((1 << DMAC_NUMBER_OF_CHANNELS) - 1)
#define BIT_PICK(N, i) ((N >> i) & 1)

static inline uint64_t read_burst_64(uint32_t base, uint32_t offset) {
  return *(volatile uint64_t*)(base + offset);
}
static inline void write_burst_64(uint32_t base, uint32_t offset, uint64_t value) {
  volatile uint64_t* localaddr = (volatile uint64_t*)(base + offset);
  *localaddr                   = value;
}
static inline uint32_t read_burst32(uint32_t base, uint32_t offset) {
  return *(volatile uint32_t*)(base + offset);
}
static inline void write_burst_32(uint32_t base, uint32_t offset, uint32_t value) {
  volatile uint32_t* localaddr = (volatile uint32_t*)(base + offset);
  *localaddr                   = value;
}
static inline uint32_t read_burst16(uint32_t base, uint32_t offset) {
  return *(volatile uint16_t*)(base + offset);
}
static inline void write_burst_16(uint32_t base, uint32_t offset, uint16_t value) {
  volatile uint16_t* localaddr = (volatile uint16_t*)(base + offset);
  *localaddr                   = value;
}

uint32_t get_free_channel(void) {
  uint64_t ch_en_reg = read_burst_64(VENUS_DMAC_ADDR, DMAC_CFG_REG_OFFSET);
  uint32_t free_channel_index;
  // while(READDATA64[(DMAC_NUMBER_OF_CHANNELS-1):0]=={(DMAC_NUMBER_OF_CHANNELS){1'b1}})
  while ((ch_en_reg & CHANNEL_MASK) == CHANNEL_MASK) {
    printf("[Hardware] SCHEDULER: Waiting for an available DMAC channel...\n");
  }
  for (int i = 0; i < DMAC_NUMBER_OF_CHANNELS; i = i + 1) {
    // READDATA64[i]==1'b0
    if (BIT_PICK(ch_en_reg, i) == 0) {
      free_channel_index = i;
      break;
    }
  }
  printf("[Hardware] SCHEDULER: DMAC channel %d is free...\n", free_channel_index);
  return free_channel_index;
  // while (tb.dut.u_venus.venus_glob_intr == 1)
  //   begin
  //       $display("[%t] [Hardware] VENUS SCHEDULER: dmac channel %d will be released soon...", $time, free_channel_index);
  //       repeat(20) @(posedge tb.dut.clk_soc_axi);
  //    end
}

/* [P.164] CHx_LLP */
void specify_first_lli(lli_t* head_lli, uint32_t free_channel_index) {
  assert((uint32_t)head_lli / 64 == 0);
  uint32_t first_lli = (uint32_t)head_lli & 0xffffffc0;
  uint64_t CHx_LLP =
      ((uint64_t)first_lli << 6) |  // LOC, Starting Address
      ((uint64_t)0 << 1) |          // Reserved and read as zero
      ((uint64_t)0 << 0);           // LMS, LLI master Select (does not exist in this design)
  write_burst_64(VENUS_DMAC_ADDR,
                 DMAC_CH_LLP_REG_OFFSET_CH(free_channel_index),
                 CHx_LLP);
}

/* [P.136]
 * DMAC_ChEnReg [15:8] -> CH_EN_WE  [7:0] -> CH_EN
 * 0: DW_axi_dmac Channel is disabled.
 * 1: DW_axi_dmac Channel is enabled.
 */
void enable_channel(uint32_t free_channel_index) {
  uint16_t CH_EN             = 1 << free_channel_index;
  uint16_t CH_EN_WE          = 1 << (free_channel_index + 8);
  uint16_t DMAC_CHEnReg_15_0 = CH_EN | CH_EN_WE;
  write_burst_16(VENUS_DMAC_ADDR,
                 DMAC_CH_EN_REG_OFFSET,
                 DMAC_CHEnReg_15_0);
}

