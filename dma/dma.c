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
extern void DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index);
extern void DMAC_CHx_enable_channel(uint32_t free_channel_index);
extern void DMAC_reset(void);
extern void DMAC_config(void);
#ifdef SIMULATE_QEMU
extern void DMAC_free_channel_init(void);
#endif

static lli_t* head_lli;
static lli_t* last_lli;
static list_t* token_list;
static block_t* current_block;

msg_t* msg_array[DMAC_NUMBER_OF_CHANNELS];

static msg_t msg_0;
static msg_t msg_1;
static msg_t msg_2;
static msg_t msg_3;
static msg_t msg_4;
static msg_t msg_5;
static msg_t msg_6;
static msg_t msg_7;

void msg_array_init(void) {
  msg_array[0] = &msg_0;
  msg_array[1] = &msg_1;
  msg_array[2] = &msg_2;
  msg_array[3] = &msg_3;
  msg_array[4] = &msg_4;
  msg_array[5] = &msg_5;
  msg_array[6] = &msg_6;
  msg_array[7] = &msg_7;
}

static inline void print_lli_info(void) {
  lli_t* current_lli = head_lli;
  uint32_t llp       = (uint32_t)current_lli->CHx_LLP;

  while ((uint32_t)current_lli != llp) {
    printf("\n--------------------------------------------\n");
    printf("Reserved0                   %x%x\n", (uint32_t)(current_lli->Reserved0 >> 32), (uint32_t)(current_lli->Reserved0));
    printf("CHx_LLP_STATUS              %x%x\n", (uint32_t)(current_lli->CHx_LLP_STATUS >> 32), (uint32_t)(current_lli->CHx_LLP_STATUS));
    printf("WB_CHx_DSTAT | WB_CHx_SSTAT %x%x\n", current_lli->WB_CHx_DSTAT, current_lli->WB_CHx_SSTAT);
    printf("CHx_CTL                     %x%x\n", (uint32_t)(current_lli->CHx_CTL >> 32), (uint32_t)(current_lli->CHx_CTL));
    printf("CHx_LLP                     %x%x\n", (uint32_t)(current_lli->CHx_LLP >> 32), (uint32_t)(current_lli->CHx_LLP));
    printf("Reserved1 | CHx_BLOCK_TS    %x%x\n", current_lli->Reserved1, current_lli->CHx_BLOCK_TS);
    printf("CHx_DAR                     %x%x\n", (uint32_t)(current_lli->CHx_DAR >> 32), (uint32_t)(current_lli->CHx_DAR));
    printf("CHx_SAR                     %x%x\n", (uint32_t)(current_lli->CHx_SAR >> 32), (uint32_t)(current_lli->CHx_SAR));
    printf("--------------------------------------------\n");
    current_lli = (lli_t*)llp;
    llp         = (uint32_t)current_lli->CHx_LLP;
  }
  printf("\n--------------------------------------------\n");
  printf("Reserved0                   %x%x\n", (uint32_t)(current_lli->Reserved0 >> 32), (uint32_t)(current_lli->Reserved0));
  printf("CHx_LLP_STATUS              %x%x\n", (uint32_t)(current_lli->CHx_LLP_STATUS >> 32), (uint32_t)(current_lli->CHx_LLP_STATUS));
  printf("WB_CHx_DSTAT | WB_CHx_SSTAT %x%x\n", current_lli->WB_CHx_DSTAT, current_lli->WB_CHx_SSTAT);
  printf("CHx_CTL                     %x%x\n", (uint32_t)(current_lli->CHx_CTL >> 32), (uint32_t)(current_lli->CHx_CTL));
  printf("CHx_LLP                     %x%x\n", (uint32_t)(current_lli->CHx_LLP >> 32), (uint32_t)(current_lli->CHx_LLP));
  printf("Reserved1 | CHx_BLOCK_TS    %x%x\n", current_lli->Reserved1, current_lli->CHx_BLOCK_TS);
  printf("CHx_DAR                     %x%x\n", (uint32_t)(current_lli->CHx_DAR >> 32), (uint32_t)(current_lli->CHx_DAR));
  printf("CHx_SAR                     %x%x\n", (uint32_t)(current_lli->CHx_SAR >> 32), (uint32_t)(current_lli->CHx_SAR));
  printf("--------------------------------------------\n");
}

/* Function: After linked-list completed construction, find a free DMA channel to transfer */
void dma_transfer_channel(void) {
#ifdef DEBUG_DMA
  print_lli_info();
#endif
  uint32_t free_channel_index = DMAC_get_free_channel();
  while (free_channel_index == DMAC_NUMBER_OF_CHANNELS) {
    // there is no free channel, hang on checking...
    free_channel_index = DMAC_get_free_channel();
  }

  /* configure and activate DMA */
  cfg_config(free_channel_index);
  DMAC_CHx_specify_first_lli(head_lli, free_channel_index);
  DMAC_CHx_enable_channel(free_channel_index);

  /* record relative DMA transfer information */
  msg_t* msg      = msg_array[free_channel_index];
  msg->lli        = head_lli;
  msg->block      = current_block;
  msg->token_list = token_list;

  /* reset head_lli */
  head_lli   = NULL;
  token_list = NULL;
}

void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, token_t* token) {
  /* create linked list for DMA transfer */
  lli_t* current_lli            = malloc_LLI();
  uint64_t destination_addr     = (uint64_t)dst;
  uint64_t source_addr          = (uint64_t)src;
  uint32_t transfer_length_byte = len;
  uint32_t total_chunk          = (transfer_length_byte / DMA_MAX_TRANSFER_LENGTH) + 1;
  uint64_t current_source_addr  = source_addr;
  uint64_t left_transfer_length = transfer_length_byte;

  // if it's not the first data chunk of this DMA channel transfer round
  if (head_lli != NULL) {
    // then link last lli with current first lli
    uint32_t next_item_pointer = (uint32_t)current_lli;
    last_lli->CHx_LLP          = (uint64_t)next_item_pointer;
  } else {
    head_lli   = current_lli;
    token_list = create_list();
  }
  // put current transfering token into global token list
  assert(token_list != NULL);
  if (token != NULL)
    insert(token_list, create_node((uint32_t)token));

  // if not the last data chunk of this DMA channel transfer round
  if (block == NULL) {
    for (int i = 0; i < total_chunk; i++) {
      // if its the last chunk to be transmitted
      if (i == total_chunk - 1) {
        // the last data chunk of this data, wait next DMA transfer link to construct
        lli_setup(destination_addr,
                  current_source_addr,
                  left_transfer_length,
                  current_lli,
                  current_lli,
                  NOT_LAST_SHADOW_REGISTER);
        last_lli = current_lli;
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
  } else {
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
    current_block = block;
    dma_transfer_channel();
  }
}

void dma_init(void) {
  DMAC_reset();
  DMAC_config();
  msg_array_init();
#ifdef SIMULATE_QEMU
  DMAC_free_channel_init();
#endif
}

void dma_test(void) {
  // uint32_t i_spm_addr = 0x99999999;
  // uint32_t task_addr  = 0x33333333;
  // uint32_t task_len   = 17000;
  // dma_transfer(i_spm_addr, task_addr, task_len);
  // dma_transfer_done_handler(2);
}

