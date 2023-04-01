#include "dma/common.h"
#include "dma/msg.h"

/* CHx_CTL in bits 62: SHADOWREG_OR_LLI_LAST [P.150] */
#define NOT_LAST_SHADOW_REGISTER 0
#define LAST_SHADOW_REGISTER     1

/* lli_create.c */
extern void lli_setup(uint64_t destination_addr, uint64_t source_addr, uint32_t transfer_length_byte, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last);
/* chx_cfg.c */
extern void cfg_config(uint32_t free_channel_index);
/* phy_interface.c */
extern uint32_t DMAC_get_free_channel(void);
extern void     DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
extern void     DMAC_CHx_enable_channel(uint32_t free_channel_index);
extern void     DMAC_reset(void);
extern void     DMAC_config(void);

static lli_t*   head_lli;
static uint64_t destination_addr;
static uint64_t source_addr;
static uint32_t transfer_length_byte;
static msg_t*   msg_array[DMAC_NUMBER_OF_CHANNELS];

static msg_t msg_0;
static msg_t msg_1;
static msg_t msg_2;
static msg_t msg_3;
static msg_t msg_4;
static msg_t msg_5;
static msg_t msg_6;
static msg_t msg_7;

static inline void msg_array_init(void) {
  msg_array[0] = &msg_0;
  msg_array[1] = &msg_1;
  msg_array[2] = &msg_2;
  msg_array[3] = &msg_3;
  msg_array[4] = &msg_4;
  msg_array[5] = &msg_5;
  msg_array[6] = &msg_6;
  msg_array[7] = &msg_7;
}

static inline void lli_link(void) {
  lli_t*   current_lli          = head_lli;
  uint32_t total_chunk          = (transfer_length_byte / DMA_MAX_TRANSFER_LENGTH) + 1;
  uint64_t current_source_addr  = source_addr;
  uint64_t left_transfer_length = transfer_length_byte;
  for (int i = 0; i < total_chunk; i++) {
    // if its the last chunk to be transmitted
    if (i == total_chunk - 1) {
      lli_setup(destination_addr,
                current_source_addr,
                left_transfer_length,
                current_lli,
                current_lli,
                LAST_SHADOW_REGISTER);
    } else {
      lli_t* next_lli = malloc_LLI();
      lli_setup(destination_addr,
                current_source_addr,
                DMA_MAX_TRANSFER_LENGTH,
                current_lli,
                next_lli,
                NOT_LAST_SHADOW_REGISTER);
      current_source_addr  = current_source_addr + 4 * (DMA_MAX_TRANSFER_LENGTH);
      left_transfer_length = left_transfer_length - DMA_MAX_TRANSFER_LENGTH;
      current_lli          = next_lli;
    }
  }
}

void dma_transfer_done_handler(uint32_t channel_index) {
  msg_t*   msg         = msg_array[channel_index];
  lli_t*   lli         = msg->lli;
  uint32_t llp         = (uint32_t)msg->lli->CHx_LLP;
  lli_t*   nxt_lli     = (lli_t*)llp;
  // uint32_t source_addr = msg->source_addr;
  while (lli != nxt_lli) {
    free_LLI(lli);
#ifdef DEBUG_DMA
    printf("SCHEDULER: Freeing LLI descriptor %p\n", lli);
#endif
    lli     = nxt_lli;
    llp     = (uint32_t)msg->lli->CHx_LLP;
    nxt_lli = (lli_t*)llp;
  }
#ifdef DEBUG_DMA
  printf("SCHEDULER: Freeing Last LLI descriptor %p\n", lli);
#endif
  free_LLI(lli);
  /*
   * TODO:
   *   judge the source_addr
   *      - activate block (scheduler should told block to start computing)
   *      - pass result to the successor (for now we cannot ensure after DMA transmit back done, then add into successor's dep fifo)
   */
}

void dma_init(void) {
  DMAC_reset();
  DMAC_config();
  msg_array_init();
}

void dma_transfer(uint32_t dst, uint32_t src, uint32_t len) {
  head_lli             = malloc_LLI();
  destination_addr     = (uint64_t)dst;
  source_addr          = (uint64_t)src;
  transfer_length_byte = len;
  lli_link();
  uint32_t free_channel_index = DMAC_get_free_channel();
  cfg_config(free_channel_index);
  DMAC_CHx_specify_first_lli(head_lli, free_channel_index);
  DMAC_CHx_enable_channel(free_channel_index);
  msg_t* msg = msg_array[free_channel_index];
  msg->lli   = head_lli;
  msg->source_addr = src;
}

void dma_test(void) {
  uint32_t i_spm_addr = 0x99999999;
  uint32_t task_addr  = 0x33333333;
  uint32_t task_len   = 17000;
  dma_transfer(i_spm_addr, task_addr, task_len);
  dma_transfer_done_handler(2);
}
