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

extern msg_t* msg_array[DMAC_NUMBER_OF_CHANNELS];

static lli_t* current_lli;
static lli_t* last_lli;
static uint32_t free_channel_index;
static uint32_t token_index;
static uint32_t lli_index;
static msg_t* msg;

void dma_init(void) {
  DMAC_reset();
  DMAC_config();
}

inline void lli_setup(uint64_t destination_addr, uint64_t source_addr, uint32_t transfer_length_byte, lli_t* current_lli, lli_t* next_lli, uint32_t lli_last) {
  lli_t* lli                 = current_lli;
  uint32_t next_item_pointer = (uint32_t)next_lli;
  lli->Reserved0             = 0;
  lli->CHx_LLP_STATUS        = 0;
  lli->WB_CHx_DSTAT          = 0;
  lli->WB_CHx_SSTAT          = 0;
  if (lli_last) {
    lli->CHx_CTL = 0xc0ffffc040089b00;
  } else {
    lli->CHx_CTL = 0x80ffffc040089b00;
  }
  lli->CHx_LLP      = (uint64_t)next_item_pointer;
  lli->CHx_BLOCK_TS = transfer_length_byte / 8;  // the [source_transfer_width]th power of 2
  lli->CHx_DAR      = destination_addr;
  lli->CHx_SAR      = source_addr;
}

inline static lli_t* create_LLI() {
  lli_index++;
  return (lli_t*)(LLI_CH(free_channel_index) + LLI_SIZE * lli_index);
}

void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, cluster_t* cluster, token_t* token) {
  // printf("dst: %p, src: %p, len: %p, block: %p, token: %p\n", dst, src, len, block, token);
  /* create linked list for DMA transfer */
  uint64_t destination_addr         = (uint64_t)dst;
  uint64_t source_addr              = (uint64_t)src;
  uint32_t transfer_length_byte     = len - 8;
  uint32_t total_chunk              = (transfer_length_byte / DMA_MAX_TRANSFER_LENGTH) + 1;
  uint64_t current_source_addr      = source_addr;
  uint64_t current_destination_addr = destination_addr;
  uint64_t left_transfer_length     = transfer_length_byte;

  if (current_lli == NULL) {
    // the first transfer task, get a free DMA channel
    free_channel_index = DMAC_get_free_channel();
    current_lli        = (lli_t*)LLI_CH(free_channel_index);
    msg                = msg_array[free_channel_index];
    token_index        = 0;
    lli_index          = 0;
  } else {
    // if it's not the first data chunk of this DMA channel transfer round, then link last lli with current first lli
    current_lli                = create_LLI();
    uint32_t next_item_pointer = (uint32_t)current_lli;
    last_lli->CHx_LLP          = (uint64_t)next_item_pointer;
  }

  // put current transfering token into global token list
  if (token != NULL) {
    msg->token_array[token_index] = (uint32_t)token;
    token_index++;
  }

  // if not the last data chunk of this DMA channel transfer round
  if (cluster == NULL) {
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
        lli_t* next_lli = create_LLI();
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
        lli_t* next_lli = create_LLI();
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
    // last data to transfer, bind relative descripotr and enable DMA channel...
    msg->cluster                  = cluster;
    msg->token_array[token_index] = LAST_TOKEN;
    CFG_config(free_channel_index);
    DMAC_CHx_specify_first_lli((lli_t*)LLI_CH(free_channel_index), free_channel_index);
    DMAC_CHx_enable_channel(free_channel_index);
    // reset
    current_lli = NULL;
  }
}

