#include "task.h"

/*
 * Note: The following data is simulated
 *      1. SPMx_ADDR --- <saddr.h>:
 *		we should know the mapping address of every block's spm
 *      2. DATAx_ADDR --- <daddr.h>:
 *		we should told DMA the destination of the data moving to the block
 */

extern void task1_exe(actor_t *g);
extern void task2_exe(actor_t *g);
extern void task3_exe(actor_t *g);
extern void task4_exe(actor_t *g);
extern void task5_exe(actor_t *g);
extern void task6_exe(actor_t *g);

void task_bind(actor_t *g, block_f *n_block, Taskfunc task_addr) {
  actor_t *actor = g;
  printf("\nBLOCK: Computing task...\n");
  n_block->task_addr = (uint32_t)task_addr;
  n_block->actor = actor;
}

/*
 * Function: 
 *	1. Allocate some heap space for data restore accroding to [result_len] attributes. 
 *	2. Inform DMA to move the result back to allocated DDR space. (pointer *p)
 *	3. Pass the result's pointer to sucessor's fifo-queue.
 */
void alloc_result(actor_t *g) {
  void *p = malloc(g->result_len);
  uint32_t alloc_addr = (uint32_t)p;
  // told DMA where to move and store the result
  dma_result(alloc_addr, DATA1_ADDR, g->result_len);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  // bind the data with it's poitner and length
  for(int i=0;i<g->nxt_num;i++){
    put_data(g->out[i], alloc_addr, g->result_len);
  }
}

/*
 * Function:
 *	Simulate block's behaviour, DO NOT exist in real scheme.
 * Note:
 *	When to free dependencies' memory? 
 *	In the current programme, I free the pointer in taskn_exe().
 *	We can put it in DMA transfer later.
 */
void block_sim(actor_t *g) {
  printf("task num: %d\n", g->task_addr);
  switch (g->task_addr) {
  case 1:
    task1_exe(g);
    break;
  case 2:
    task2_exe(g);
    break;
  case 3:
    task3_exe(g);
    break;
  case 4:
    task4_exe(g);
    break;
  case 5:
    task5_exe(g);
    break;
  case 6:
    task6_exe(g);
    break;
  default:
    printf("\nERROR: No task code matching...\n");
    break;
  }
}

/*
 * Function:
 *	1. Top-level function to handle result and recycle block.
 *	2. Flush block's flag after block recycle and result allocation.
 */
void callback(actor_t *g) {
  block_sim(g);
  printf("\nSCHEDULER: Allocating result...\n");
  alloc_result(g);
  printf("SCHEDULER: The result data has been recycled...\n");
}

/*
 * Function:
 *	Check whether this task could be fired
 *	Which means all the predecessors finished their execution
 */
uint8_t actor_ready(actor_t *g) {
  uint8_t flag = 1;
  // visit all dependencies' fifo
  for (int i = 0; i < g->dep_num; i++) {
    flag = flag && fifo_ready(g->in[i]);
  }
  if (flag) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t successor_full(actor_t *g) {
  uint8_t flag = 1;
  for (int i = 0; i< g->nxt_num; i++) {
    flag = flag && fifo_full(g->out[i]);
  }
  if (flag) { return 1; } else { return 0; }
}


/*
 * Function:
 *	1. The Top-level function of dependency-checking process
 *	2. Inform DMA move the code and data
 *	3. associate current block and task
 */
void task(actor_t *g, block_f *n_block) {
  /*
   *  1.1 check firing condition
   *  1.2 catch idle block
   *  1.3 check the successor's fifo is full or not
   */
   //printf("a %d, f %d \n",actor_ready(g),fifo_full(g->out));
  if (actor_ready(g) && !successor_full(g)) {
    printf("TASK: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be inflight status
    _set_block_flag(n_block, BLOCK_INFLIGHT);
    // 3. inform revelant task descriptor to DMA
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    for (uint32_t i = 0; i < g->dep_num; i++) {
      data_t data = get_data(g->in[i]);
      dma_data(DATA1_ADDR, data.ptr, data.len);
    }
    // 4. associate block and task
    task_bind(g, n_block, &callback);
  }
}
