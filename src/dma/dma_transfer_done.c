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
static uint32_t ideal_block;

static void scheduler_pass_result() {
  actor_t* actor = block->actor;
  node_t* p      = token_list->tail->prev;

  // pass the result to successors i -> different result |  j -> different fifo
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    // real token
    if (p != token_list->head) {
      for (int j = 0; actor->out[i][j] != NULL; j++) {
        token_t* original_token = (token_t*)p->item;
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
    } else {
      // pseudo token (dynamic dependencies)
      for (int j = 0; actor->out[i][j] != NULL; j++) {
        // create a pseudo_token and inject it into pseudo task's fifo
        token_t* pseudo_token = (token_t*)PSEUDO_TOKEN_LABEL;
        put_token(actor->out[i][j], pseudo_token);
      }
    }
  }

  // release expired data
  node_t* fire_node = actor->fire_list->tail->prev;
  destroy_node(fire_node);
  if (actor->linger_list != NULL) {
    node_t* linger_node = actor->linger_list->head->next;
    linger_t* linger    = (linger_t*)linger_node->item;
    destroy_node(linger_node);
    free(linger);
  }

  //  continue to check next expected arrival
  if (is_list_empty(actor->fire_list) == 0)
    ideal_block = read_last(actor->fire_list)->item;
}

static inline void check_if_done(node_t* p) {
  linger_t* linger = (linger_t*)(p->item);
  if ((uint32_t)linger->block == ideal_block) {
    token_list = linger->token_list;
    scheduler_pass_result();
  }
  // if dismatch, visit next
}

static inline void linger_insert() {
  // bind interrupt block with current actor's linger list
  if (block->actor->linger_list == NULL)
    block->actor->linger_list = create_list();
  linger_t* linger   = malloc(sizeof(linger_t));
  linger->block      = block;
  linger->token_list = token_list;
  insert(block->actor->linger_list, create_node((uint32_t)linger));
}

static uint32_t result_deliver() {
  actor_t* actor = block->actor;

  ideal_block = read_last((actor->fire_list))->item;

  linger_insert();

  if ((uint32_t)block == ideal_block) {
    // pass the result to the successor
    scheduler_pass_result();
    if (actor->linger_list == NULL)
      return 0;
    // check the rest of lingers
    while (!is_list_empty(actor->linger_list)) {
      traverse(actor->linger_list, check_if_done);
    }
    return 0;
  } else {
    return 1;
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
  // free the hole list (node + list)
  destroy_list(token_list);
}

void dma_transmit_done_handler(uint32_t channel_index) {
  /* catch current DMA's transfer information */
  msg_t* msg     = msg_array[channel_index];
  lli_t* lli     = msg->lli;
  uint32_t llp   = (uint32_t)msg->lli->CHx_LLP;
  lli_t* nxt_lli = (lli_t*)llp;
  block          = msg->block;
  token_list     = msg->token_list;
  /* Note that by default, this interrupt is not triggered repeatedly.
   * Which means one transfer would not be triggered more than once,
   * otherwise free_LLI would free a free memory block.
   */

  /* free this transfer's linked list */
  while (lli != nxt_lli) {
    free_LLI(lli);
    lli     = nxt_lli;
    llp     = (uint32_t)msg->lli->CHx_LLP;
    nxt_lli = (lli_t*)llp;
  }
  free_LLI(lli);

  /* judge whether code/data transfer OR result transfer */
  if ((block->flags & BLOCK_RESULT) == 0) {
    // activate corresponding block
    WRITE_BURST_32(block->base_addr, BLOCK_CTRLREGS_OFFSET, EN_SOFT_RST);
    recycle_garbage();
  } else {
    // pass result to the successor
    uint32_t disorder = result_deliver();
    // reset the block's status flag
    _clear_block_flag(block);
    // if arrival disorder then skip ready search step
    if (!disorder)
      ready_search();
  }
}

