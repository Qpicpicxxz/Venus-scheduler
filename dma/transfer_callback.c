#include "dma/common.h"
#include "dma/msg.h"
#include "task.h"

/* dma.c */
extern msg_t* msg_array[DMAC_NUMBER_OF_CHANNELS];
/* fire_check.c */
extern void ready_search(void);
/* phy_interface.c */
#ifdef SIMULATE_QEMU
extern void set_channel_idle(uint32_t index);
#endif

/* internal variables */
static block_t* block;
// static data_t* data;
static list_t* data_list;
static uint32_t ideal_block;

static void scheduler_pass_result() {
#ifdef DEBUG_SCHEDULER
  printf("SCHEDULER: Pass result to the successor...\n");
#endif
  actor_t* actor = block->actor;
  node_t* p      = data_list->tail->prev;

  // pass the result to successors i -> different result |  j -> different fifo
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    assert(p != data_list->head);
    for (int j = 0; actor->out[i][j] != NULL; j++)
      put_data(actor->out[i][j], (data_t*)p->item);
    p = p->prev;
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
#ifdef DEBUG_SCHEDULER
    printf("SCHEDULER: Linger result is found...\n");
#endif
    data_list = linger->data_list;
    scheduler_pass_result();
  }
  // if dismatch, visit next
}

static inline void linger_insert() {
  // bind interrupt block with current actor's linger list
  if (block->actor->linger_list == NULL)
    block->actor->linger_list = create_list();
  linger_t* linger  = malloc(sizeof(linger_t));
  linger->block     = block;
  linger->data_list = data_list;
  insert(block->actor->linger_list, create_node((uint32_t)linger));
}

static void result_deliver() {
  actor_t* actor = block->actor;

  ideal_block = read_last((actor->fire_list))->item;

  linger_insert();

  if ((uint32_t)block != ideal_block) {
#ifdef DEBUG_SCHEDULER
    printf(PINK("SCHEDULER: Oops! Wrong result arrival sequence...\n"));
#endif
  } else {
    // pass the result to the successor
    scheduler_pass_result();
    if (actor->linger_list == NULL)
      return;
    // check the rest of lingers
    while (!is_list_empty(actor->linger_list)) {
      traverse(actor->linger_list, check_if_done);
    }
  }
}

static inline void recycle_garbage(void) {
  data_t* data;
  for (node_t* p = data_list->head->next; p != data_list->tail; p = p->next) {
    data = (data_t*)p->item;
    if (data->cnt == 1) {
      // garbage collection
      printf(YELLOW("last use...\n"));
      free((void*)data->ptr);
      free((void*)data);  // free data flag space
    } else {
      // upate reference count (lifecycle)
      data->cnt--;
    }
  }
  destroy_list(data_list);
}

void dma_transfer_done_handler(uint32_t channel_index) {
  /* catch current DMA's transfer information */
  msg_t* msg     = msg_array[channel_index];
  lli_t* lli     = msg->lli;
  uint32_t llp   = (uint32_t)msg->lli->CHx_LLP;
  lli_t* nxt_lli = (lli_t*)llp;
  block          = msg->block;
  data_list      = msg->data_list;
#ifdef SIMULATE_QEMU
  set_channel_idle(channel_index);
  printf("DMA channel %d is free\n", channel_index);
#endif
  /* Note that by default, this interrupt is not triggered repeatedly.
   * Which means one transfer would not be triggered more than once,
   * otherwise free_LLI would free a free memory block.
   */

  /* free this transfer's linked list */
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

  /* judge whether code/data transfer OR result transfer */
  if ((block->flags & BLOCK_RESULT) == 0) {
    recycle_garbage();
    // TODO: activate corresponding block
    // block_activate();
#ifdef DEBUG_SCHEDULER
    actor_t* actor       = block->actor;
    uint32_t block_index = ((uint32_t)block - block_start) / sizeof(block_t) + 1;
    uint32_t actor_index = ((uint32_t)actor - actor_start) / actor_space;

    printf("DMA: Channel transfer done, activate block...\n");
    printf("BLOCK %d: Computing task %c...\n", block_index, actor_index + 65);
#endif
  } else {
    // TODO: pass result to the successor
    // data = (data_t*)data_list->head->next->item;
    result_deliver();
#ifdef DEBUG_SCHEDULER
    printf("DMA: Result transfer done...\n");
#endif
    // reset the block's status flag
    _clear_block_flag(block);
    ready_search();
  }
}
