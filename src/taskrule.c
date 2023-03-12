#include "task.h"

/* Block compute process callback */
extern void task1_exe(actor_t *g);
extern void task2_exe(actor_t *g);
extern void task3_exe(actor_t *g);
extern void task4_exe(actor_t *g);
extern void task5_exe(actor_t *g);
extern void task6_exe(actor_t *g);

/* A handler to bind inflight task with current block */
void task_bind(actor_t *g, block_f *n_block, Taskfunc task_addr) {
  actor_t *actor = g;
  printf("\nBLOCK: Computing task...\n");
  n_block->task_addr = (uint32_t)task_addr;
  n_block->actor = actor;
}

/*
 * Function:
 *	1. Allocate some heap space for data restore accroding to [result_len] attributes.
 *	2. Inform DMA to move the result back to allocated DDR space. (pointer)
 *	3. Pass the result's pointer to sucessor's fifo-queue.
 */
void alloc_result(actor_t *g) {
  void *p;
  // 1. allocate data storage space
  p = malloc(g->result_len);
  *(int *)p = result;
  uint32_t alloc_addr = (uint32_t)p;
  printf("SCHEDULER: Result %d is stored in 0x%x\n", *(uint32_t *)alloc_addr,
         alloc_addr);
  // 2. allocate data descriptor space
  data_t *data;
  p = malloc(sizeof(*data));
  data = (data_t *)p;
  data->ptr = alloc_addr;	// initialize data descriptor
  data->len = g->result_len;
  data->cnt = g->nxt_num;
  // told DMA where to move and store the result
  dma_result(alloc_addr, DATA1_ADDR, g->result_len);
  // bind the data with it's poitner, length and lifecycle
  // BIG Problem: how can we guarantee data sequence?
  for (int i = 0; i < g->nxt_num; i++) {
    put_data(g->out[i], data);
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
  // block simulation done, pick out the dependency
  for (int i = 0; i < g->dep_num; i++) {
    data_t *data = get_data(&dma_trans_in);
    // check dependency's lifecycle
    if (data->cnt == 1) {
      // In real sence, we can free data space after last successor's firing
      printf("SCHEDULER: Last use, free data space...\n");
      free((void *)data->ptr);	// free data space
      free((void *)data);	// free data flag space
    } else {
      data->cnt--;
    }
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
    flag = flag && !fifo_empty(g->in[i]);
  }
  if (flag) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t successor_full(actor_t *g) {
  uint8_t flag = 1;
  for (int i = 0; i < g->nxt_num; i++) {
    flag = flag && fifo_full(g->out[i]);
  }
  if (flag) {
    return 1;
  } else {
    return 0;
  }
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
  if (actor_ready(g) && !successor_full(g)) {
    printf("TASK: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be inflight status
    _set_block_flag(n_block, BLOCK_INFLIGHT);
    // 3. inform task code descriptor to DMA
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    // 4. access each dependency in turn
    for (uint32_t i = 0; i < g->dep_num; i++) {
      // 4.1 get data descriptor
      data_t *data = get_data(g->in[i]);
      // 4.2 initialize data descriptor
      put_data(&dma_trans_in, data);
      // 4.3 inform data descriptor to DMA
      dma_data(DATA1_ADDR, data->ptr, data->len);
    }
    // 5. associate block and task
    task_bind(g, n_block, &callback);
  }
}
