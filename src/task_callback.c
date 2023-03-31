#include "task.h"

/* fireCheck.c */
extern void ready_search(void);
/* dma.c */
extern void dma_result(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);
/* Some inner variables */
static uint32_t ideal_block;
static actor_t* cur_actor;
static data_t* cur_data;

void pass_result() {
#ifdef DEBUG_SCHEDULER
  printf("SCHEDULER: Pass result to the successor...\n");
#endif

  // pass the result to successors
  for (int i = 0; i < cur_actor->nxt_num; i++) {
    put_data(cur_actor->out[i], cur_data);
  }

  // release expired data
  node_t* fire_node = cur_actor->fire_list->tail->prev;
  node_t* linger_node = cur_actor->linger_list->head->next;
  linger_t* cur_linger = (linger_t*)linger_node->item;
  destroy_node(fire_node);
  destroy_node(linger_node);
  free(cur_linger);

  //  continue to check next expected arrival
  if (!is_list_empty(cur_actor->fire_list))
    ideal_block = read_last(cur_actor->fire_list)->item;
}

static inline void check_if_done(node_t* p) {
  linger_t* linger = (linger_t*)(p->item);
  if ((uint32_t)linger->block == ideal_block) {
#ifdef DEBUG_SCHEDULER
    printf("SCHEDULER: Linger result is found...\n");
#endif
    cur_data = linger->data;
    pass_result();
  }
  // if dismatch, visit next
}

/*
 * Function:
 *	1. Allocate some heap space for data restore accroding to [result_len] attributes
 *	2. Inform DMA to move the result back to allocated DDR space (pointer)
 *	3. Check if this result the right arrival sequence
 *	4. If the right arrival sequence, pass the result's pointer to sucessor's dependency fifo
 */
static inline void alloc_result(void) {
  // prpare return result's store space in advance
  void* p = malloc(cur_actor->result_len);
  uint32_t alloc_addr = (uint32_t)p;

  // allocate data descriptor space and initialize
  cur_data = malloc(sizeof *cur_data);
  cur_data->ptr = alloc_addr;             // data pointer
  cur_data->len = cur_actor->result_len;  // data length
  cur_data->cnt = cur_actor->nxt_num;     // data lifecycle

  // told DMA where to move and store the result
  dma_result(alloc_addr, DATA1_ADDR, cur_actor->result_len);
  // SIMULATE DMA stored the result (in a allocated memory space)
  *(int*)p = 1;
#ifdef DEBUG_SCHEDULER
  printf(BLUE("DMA: Result data is stored in 0x%x\n"), alloc_addr);
#endif

  // check whether the right arrival sequence or not
  ideal_block = read_last((cur_actor->fire_list))->item;
  linger_t* linger = (linger_t*)read_first((cur_actor->linger_list))->item;
  uint32_t actual_block = (uint32_t)linger->block;

  // if the wrong arrival sequence happen (sporadic)
  if (ideal_block != actual_block) {
#ifdef DEBUG_SCHEDULER
    printf(PINK("SCHEDULER: Oops! Wrong result arrival sequence...\n"));
#endif
    // bind store the data descripor into linger list descriptor
    linger->data = cur_data;
  } else {
    // pass the result to the successor
    pass_result();
    // check the rest of lingers
    while (!is_list_empty(cur_actor->linger_list)) {
      traverse(cur_actor->linger_list, check_if_done);
    }
  }
}

static inline void linger_insert(block_t* n_block) {
  // bind interrupt block with current actor's linger list
  if (n_block->actor->linger_list == NULL)
    n_block->actor->linger_list = create_list();
  linger_t* linger = malloc(sizeof(linger_t));
  linger->block = n_block;
  linger->data = NULL;
  insert(n_block->actor->linger_list, create_node((uint32_t)linger));
}

/*
 * Function:
 *	1. Check if the block just complete the computation
 *	2. Link current block into Linger_list (to check arrival sequence)
 *	3. Call callback()
 *	4. Flush current block's status flag
 *	5. Add current block into idle block queue
 */
void block_recycle(block_t* n_block) {
  // check if this block needs to recycle result
  if ((n_block->flags & BLOCK_INFLIGHT) != 0) {
#ifdef DEBUG_SCHEDULER
    printf(YELLOW("BLOCK %d: Job done\n"), ((uint32_t)n_block - block_start) / sizeof(block_t) +1);
#endif
    linger_insert(n_block);
    cur_actor = n_block->actor;
    alloc_result();
    // reset the block's status flag
    _clear_block_flag(n_block);
    ready_search();
  }
  // add current block into idle queue (if this block isn't in idle-fifo)
  if ((n_block->flags & BLOCK_INFIFO) == 0) {
    // mark this block has been put into idle-fifo
    _set_block_flag(n_block, BLOCK_INFIFO);
    put_queue(&block_q, (uint32_t)n_block);
  }
}

