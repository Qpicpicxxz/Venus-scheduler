#include "common.h"
#include "dma.h"
#include "hw/axi.h"
#include "hw/config.h"
#include "msg.h"
#include "task.h"

/* dma.c */
extern msg_t* msg_array[DMAC_NUMBER_OF_CHANNELS];
/* fire_check.c */
extern void ready_search(void);

/* internal variables */
static block_t* block;
// static data_t* data;
static list_t* token_list;

static void scheduler_pass_result() {
  actor_t* actor = block->actor;  // -32(s0)
  // token_list: 0x8000103a0
  node_t* p = token_list->tail->prev;  // -20(s0)
  // 0x80010f48 0x80010fc8

  // pass the result to successors i -> different result |  j -> different fifo
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    for (int j = 0; actor->out[i][j] != NULL; j++) {
      token_t* original_token = (token_t*)p->item;  // -36(s0)
      // if its the first fifo of this result
      if (j == 0) {
        put_token(actor->out[i][j], original_token);
      } else {
        // create(duplicate) a new data structure (to ensure different fifo has different data structure)
        token_t* dup_token = malloc(sizeof(token_t));
        dup_token->data    = original_token->data;
        dup_token->attr    = original_token->attr;
        put_token(actor->out[i][j], dup_token);
      }
    }
    p = p->prev;
  }
}

static inline void recycle_garbage(void) {
  token_t* token;

  for (node_t* p = token_list->head->next; p != token_list->tail; p = p->next) {
    // parse data struct
    token = (token_t*)p->item;
    if (token->data->cnt == 1) {
      // garbage collection
      free((void*)token->data->ptr);  // free real data
      free((void*)token->data);       // free data flag space
    } else {
      // upate reference count (lifecycle)
      token->data->cnt--;
    }
    free((void*)token);
  }
}

void dma_transmit_done_handler(uint32_t channel_index) {
  /* catch current DMA's transfer information */
  msg_t* msg     = msg_array[channel_index];
  lli_t* lli     = msg->lli;
  uint32_t llp   = (uint32_t)msg->lli->CHx_LLP;
  lli_t* nxt_lli = (lli_t*)llp;
  block          = msg->block;
  token_list     = msg->token_list;

  /* judge whether code/data transfer OR result transfer */
  if ((block->flags & BLOCK_RESULT) == 0) {
    // activate corresponding block (ASAP!!!)
    WRITE_BURST_32(block->base_addr, BLOCK_CTRLREGS_OFFSET, EN_SOFT_RST);
    recycle_garbage();
  } else {
    // pass result to the successor
    scheduler_pass_result();
    // reset the block's status flag
    _clear_block_flag(block);
  }

  /* free this transfer's linked list */
  while (lli != nxt_lli) {
    free_LLI(lli);
    lli     = nxt_lli;
    llp     = (uint32_t)msg->lli->CHx_LLP;
    nxt_lli = (lli_t*)llp;
  }
  free_LLI(lli);
  /* Note that by default, this interrupt is not triggered repeatedly.
   * Which means one transfer would not be triggered more than once,
   * otherwise free_LLI would free a free memory block.
   */
  reset_list(msg->token_list);
}

