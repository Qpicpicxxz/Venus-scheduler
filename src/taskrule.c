#include "task.h"

/*
 * Note: The following data is simulated
 *      1. SPMx_ADDR  ---> we should know the mapping address of every block's
 * spm --- <saddr.h>
 *      2. DATAx_ADDR ---> we should told DMA the destination of the data moving
 * to the block --- <daddr.h>
 */

static fifo_t q_block;
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
 * block_recycle(&block1)
 * block_n: address of block_n flags
 * (block_f *)block_n: original pointer
 */
void block_recycle(block_f *block_n) {
  // 1. check if this block needs to recycle result
  if (!_is_block_done(block_n)) {
    // 1.1 catch scheduler recycle handler's pointer
    Taskfunc task_callback = (Taskfunc)block_n->task_addr;
    // 1.2 call the handler with revelant actors
    task_callback(block_n->actor, block_n);
  }
  // 2. add current block into idle queue (ignore data length)
  put_data(&q_block, (uint32_t)block_n, 1);
}


void alloc_result(actor_t *g) {
  void *p = malloc(g->result_len);
  uint32_t alloc_addr = (uint32_t)p;
  // told DMA where to move and store the result
  dma_result(alloc_addr, DATA1_ADDR, g->result_len);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  // bind the data with it's poitner and length
  put_data(g->out, alloc_addr, g->result_len);
}

void task_exe(actor_t *g, block_f *n_block) {
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
  printf("\nSCHEDULER: Allocating result...\n");
  alloc_result(g);
  _clear_block_flag(n_block);
  printf("SCHEDULER: The result data has been recycled...\n");
}

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

void task(actor_t *g) {
  /*
   *  1. check firing condition
   *     catch idle block
   *     check the successor's fifo is full or not
   */
  if (actor_ready(g) && fifo_size(&q_block) >= 1 && fifo_full(g->out)) {
    block_f *n_block = (block_f *)get_ptr(&q_block);
    printf("\nTASK: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be busy status
    _set_block_flag(n_block, BLOCK_BUSY);
    // 3. pass the task code's revelent information to DMA
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    for (uint32_t i = 0; i < g->dep_num; i++) {
      data_t data = get_data(g->in[i]);
      dma_data(DATA1_ADDR, data.ptr, data.len);
    }
    // 4. associate block and task
    task_bind(g, n_block, &task_exe);
  }
}
