#include "codeaddr.h"
#include "common.h"
#include "dma.h"
#include "hw/axi.h"
#include "hw/config.h"
#include "linkedlist.h"
#include "msg.h"

/* CHx_CTL in bits 62: SHADOWREG_OR_LLI_LAST [P.150] */
#define NOT_LAST_SHADOW_REGISTER 0
#define LAST_SHADOW_REGISTER     1

extern void vcs_stop(void);

/* dma_config.c */
extern void DMAC_config(void);
extern void DMAC_reset(void);
extern uint64_t CTL_config(uint64_t lli_last);
extern void CFG_config(uint32_t free_channel_index);
/* dma_irq.c */
extern uint32_t DMAC_get_free_channel(void);
extern void DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
extern void DMAC_CHx_enable_channel(uint32_t free_channel_index);
/* start.S */
extern void vcs_stop(void);

extern msg_t* msg_array[DMAC_NUMBER_OF_CHANNELS];

static lli_t* head_lli;
static lli_t* last_lli;
static msg_t* msg;
static uint32_t free_channel_index;

uint64_t destination_max_burst_length = 256;
uint64_t source_max_burst_length      = 256;
uint32_t destination_transfer_width   = AXI4_BURST_SIZE_8_BYTE;
uint32_t source_transfer_width        = AXI4_BURST_SIZE_8_BYTE; /* SRC_TR_WIDTH */

void dma_init(void) {
  DMAC_reset();
  DMAC_config();
}

void lli_setup(uint64_t destination_addr, uint64_t source_addr, uint32_t transfer_length_byte, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last) {
  lli_t* lli                 = current_lli;
  uint32_t next_item_pointer = (uint32_t)next_lli;
  lli->Reserved0             = 0;
  lli->CHx_LLP_STATUS        = 0;
  lli->WB_CHx_DSTAT          = 0;
  lli->WB_CHx_SSTAT          = 0;
  lli->CHx_CTL               = CTL_config(lli_last);
  lli->CHx_LLP               = (uint64_t)next_item_pointer;
  lli->CHx_BLOCK_TS          = transfer_length_byte / 8;  // the [source_transfer_width]th power of 2
  lli->CHx_DAR               = destination_addr;
  lli->CHx_SAR               = source_addr;
}

void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, token_t* token) {
  /* create linked list for DMA transfer */
  lli_t* current_lli                = malloc_LLI();                                          // -36
  uint64_t destination_addr         = (uint64_t)dst;                                         // -80 -76
  uint64_t source_addr              = (uint64_t)src;                                         // -88 -84
  uint32_t transfer_length_byte     = len;                                                   // -92
  uint32_t total_chunk              = (transfer_length_byte / DMA_MAX_TRANSFER_LENGTH) + 1;  // -96
  uint64_t current_source_addr      = source_addr;
  uint64_t current_destination_addr = destination_addr;
  uint64_t left_transfer_length     = transfer_length_byte;

  // if it's not the first data chunk of this DMA channel transfer round
  if (head_lli != NULL) {
    // then link last lli with current first lli
    uint32_t next_item_pointer = (uint32_t)current_lli;
    last_lli->CHx_LLP          = (uint64_t)next_item_pointer;
  } else {
    // the first transfer task, get a free DMA channel
    free_channel_index = DMAC_get_free_channel();
    head_lli           = current_lli;
    msg                = msg_array[free_channel_index];
    msg->lli           = head_lli;
  }
  // put current transfering token into global token list
  if (token != NULL)
    insert(msg->token_list, create_node((uint32_t)token));

  // if not the last data chunk of this DMA channel transfer round
  if (block == NULL) {
    for (int i = 0; i < total_chunk; i++) {
      // if its the last chunk to be transmitted
      if (i == total_chunk - 1) {
        // the last data chunk of this data, wait next DMA transfer link to construct
        lli_setup(current_destination_addr,
                  current_source_addr,
                  left_transfer_length,
                  current_lli,
                  current_lli,
                  NOT_LAST_SHADOW_REGISTER);
        last_lli = current_lli;
      } else {
        lli_t* next_lli = malloc_LLI();
        lli_setup(current_destination_addr,
                  current_source_addr,
                  DMA_MAX_TRANSFER_LENGTH,
                  current_lli,
                  next_lli,
                  NOT_LAST_SHADOW_REGISTER);
        current_source_addr      = current_source_addr + DMA_MAX_TRANSFER_LENGTH;
        current_destination_addr = current_destination_addr + DMA_MAX_TRANSFER_LENGTH;
        left_transfer_length     = left_transfer_length - DMA_MAX_TRANSFER_LENGTH;
        current_lli              = next_lli;
      }
    }
  } else {
    for (int i = 0; i < total_chunk; i++) {
      // if its the last chunk to be transmitted
      if (i == total_chunk - 1) {
        lli_setup(current_destination_addr,
                  current_source_addr,
                  left_transfer_length,
                  current_lli,
                  current_lli,
                  LAST_SHADOW_REGISTER);
      } else {
        lli_t* next_lli = malloc_LLI();
        lli_setup(current_destination_addr,
                  current_source_addr,
                  DMA_MAX_TRANSFER_LENGTH,
                  current_lli,
                  next_lli,
                  NOT_LAST_SHADOW_REGISTER);
        current_source_addr      = current_source_addr + DMA_MAX_TRANSFER_LENGTH;
        current_destination_addr = current_destination_addr + DMA_MAX_TRANSFER_LENGTH;
        left_transfer_length     = left_transfer_length - DMA_MAX_TRANSFER_LENGTH;
        current_lli              = next_lli;
      }
    }
    // last data to transfer, enable DMA channel...
    msg->block = block;
    CFG_config(free_channel_index);
    DMAC_CHx_specify_first_lli(head_lli, free_channel_index);
    DMAC_CHx_enable_channel(free_channel_index);
    // reset
    head_lli = NULL;
  }
}

