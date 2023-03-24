#include "task.h"

/* fireCheck.c */
extern queue_t block_q;
extern void ready_search(void);
/* dma.c */
extern void dma_result(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);
/* Some inner variables */
static uint32_t ideal_block;
static uint32_t result;  // just simulation
static actor_t* cur_actor;
static data_t* cur_data;

void pass_result() {
  printf("SCHEDULER: Expected result, pass to the successor...\n");

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

void check_if_done(node_t* p) {
  linger_t* linger = (linger_t*)(p->item);
  if ((uint32_t)linger->block == ideal_block) {
    printf("SCHEDULER: Linger result %d is found...\n", *(uint32_t*)linger->data->ptr);
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
void alloc_result(void) {
  printf("\nSCHEDULER: Allocating result...\n");

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
  *(int*)p = result;
  printf(BLUE("DMA: Result %d is stored in 0x%x\n"), *(uint32_t*)alloc_addr, alloc_addr);

  // check whether the right arrival sequence or not
  ideal_block = read_last((cur_actor->fire_list))->item;
  linger_t* linger = (linger_t*)read_first((cur_actor->linger_list))->item;
  uint32_t actual_block = (uint32_t)linger->block;

  // if the wrong arrival sequence happen (sporadic)
  if (ideal_block != actual_block) {
    printf(PINK("SCHEDULER: Oops! Wrong result arrival sequence...\n"));
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

void linger_insert(block_f* n_block) {
  // bind interrupt block with current actor's linger list
  if (n_block->actor->linger_list == NULL)
    n_block->actor->linger_list = create_list();
  printf("SCHEDULER: Inserting block into finish list...\n");
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
void block_recycle(block_f* n_block) {
  // SIMULATE ONLY: pass the result here
  result = n_block->result;
  // check if this block needs to recycle result
  if ((n_block->flags & BLOCK_INFLIGHT) != 0) {
    printf(YELLOW("\nBLOCK 0x%x: Job done, result is: %d\n"), n_block, result);
    linger_insert(n_block);
    cur_actor = n_block->actor;
    alloc_result();
    printf("SCHEDULER: Callback handler done...\n");
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

