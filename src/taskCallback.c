#include "task.h"

/* fireCheck.c */
extern queue_t block_q;
extern void ready_search(void);
/* dma.c */
extern void dma_result(uint32_t data_dst, uint32_t data_addr,
                       uint32_t data_len);
/* Some inner variables */
static uint32_t ideal_block;
static uint32_t result; // just simulation
static actor_t *cur_actor;
static data_t *cur_data;


/*
 * Function:
 *	1. Check if the block just complete the computation
 *	2. Link current block into Linger_list (to check arrival sequence)
 *	3. Call callback()
 *	4. Flush current block's status flag
 *	5. Add current block into idle block queue
 */
void block_recycle(block_f *n_block) {
  // 0. SIMULATE ONLY: pass the result here
  result = n_block->result;
  // 1. check if this block needs to recycle result
  if ((n_block->flags & BLOCK_INFLIGHT) != 0) {
    printf(YELLOWSET "\nBLOCK 0x%x:", n_block);
    printf(" Job done, result is: %d\n" RESET, result);
    // 1.1 bind interrupt block with current actor's linger list
    if (n_block->actor->linger_list == NULL)
      n_block->actor->linger_list = create_list();
    printf("SCHEDULER: Inserting block into finish list...\n");
    // 1.1.1 initialize a linger struct
    linger_t *linger = malloc(sizeof(linger_t));
    printf("alloc linger pointer: 0x%x\n", linger);
    linger->block = n_block;
    linger->data = NULL;
    // 1.1.2 put linger's address as a node item and insert into linger list
    node_t *p = create_node((uint32_t)linger);
    insert(n_block->actor->linger_list, p);
    // newly added node would be placed head->next, which is read_first
    linger = (linger_t *)read_first((n_block->actor->linger_list))->item;
    // 1.2 call the handler with revelant actors
    cur_actor = n_block->actor;
    alloc_result();
     printf("SCHEDULER: Callback handler done...\n");
    // 3. reset the block's status flag
    _clear_block_flag(n_block);
    ready_search();
  }
  // 2. add current block into idle queue (if this block isn't in idle-fifo)
  if ((n_block->flags & BLOCK_INFIFO) == 0) {
    // mark this block has been put into idle-fifo
    _set_block_flag(n_block, BLOCK_INFIFO);
    put_queue(&block_q, (uint32_t)n_block);
  }
}

void pass_result() {
  // 5.4.1 pass the result to the successor
  printf("SCHEDULER: Expected result, pass to the successor...\n");
  for (int i = 0; i < cur_actor->nxt_num; i++) {
    put_data(cur_actor->out[i], cur_data);
  }
  // 5.4.2 release passed result (tail->prev)
  
  node_t *fire_node = cur_actor->fire_list->tail->prev;
  printf("fire node 0x%x\n", fire_node);
  delete_node(fire_node);
  free_node(fire_node);
  
  node_t *linger_node = cur_actor->linger_list->head->next;
  linger_t *cur_linger = (linger_t *)linger_node->item;
  printf("linger node 0x%x\n", linger_node);
  printf("linger 0x%x\n", cur_linger);
  delete_node(linger_node);
  free_node(linger_node);
  free(cur_linger);
  //delete_node(cur_actor->linger_list->head->next);
  //free_node(cur_actor->linger_list->head->next);
  // 5.4.3 deassign next expected arrival
  if (!is_list_empty(cur_actor->fire_list))
    ideal_block = read_last(cur_actor->fire_list)->item;
}

void check_if_done(node_t *p) {
  linger_t *linger = (linger_t *)(p->item);
  // 5.5.1 if this linger result could be passed
  if ((uint32_t)linger->block == ideal_block) {
    printf("SCHEDULER: Linger result %d is found...\n",
           *(uint32_t *)linger->data->ptr);
    // 5.5.2 assign pass data
    cur_data = linger->data;
    // 5.5.3 pass this result
    pass_result();
  }
  // 5.5.4 if dismatch, visit next
}

/*
 * Function:
 *	1. Allocate some heap space for data restore accroding to [result_len]
 *attributes
 *	2. Inform DMA to move the result back to allocated DDR space (pointer)
 *	3. Check if this result the right arrival sequence
 *	4. If the right arrival sequence, pass the result's pointer to
 *sucessor's dependency fifo
 */
void alloc_result() {
  printf("\nSCHEDULER: Allocating result...\n");
  void *p;
  // 1. allocate data storage space
  p = malloc(cur_actor->result_len);
  uint32_t alloc_addr = (uint32_t)p;
  // 2. allocate data descriptor space
  cur_data = malloc(sizeof *cur_data);
  // 3. initialize data descriptor
  cur_data->ptr = alloc_addr;            // data pointer
  cur_data->len = cur_actor->result_len; // data length
  cur_data->cnt = cur_actor->nxt_num;    // data lifecycle
  // 4. told DMA where to move and store the result
  dma_result(alloc_addr, DATA1_ADDR, cur_actor->result_len);
  // 4.1 SIMULATE DMA stored the result
  *(int *)p = result;
  printf(BLUESET "DMA: Result %d is stored in 0x%x\n" RESET,
         *(uint32_t *)alloc_addr, alloc_addr);
  // 5. check whether the right arrival sequence or not
  // 5.1 catch the block we should have
  ideal_block = read_last((cur_actor->fire_list))->item;
  // 5.2 catch the block just push interrupt
  linger_t *linger = (linger_t *)read_first((cur_actor->linger_list))->item;
  uint32_t actual_block = (uint32_t)linger->block;
  // 5.3 if the wrong arrival sequence happen (sporadic)
  if (ideal_block != actual_block) {
    printf(PINK("SCHEDULER: Oops! Wrong result arrival sequence...\n"));
    // 5.3.1 bind store the data descripor into linger list descriptor
    linger->data = cur_data;
  } else {
    // 5.4 pass the result to the successor
    pass_result();
    // 5.5 check the rest of lingers
    while (!is_list_empty(cur_actor->linger_list)) {
      traverse(cur_actor->linger_list, check_if_done);
    }
  }
}

