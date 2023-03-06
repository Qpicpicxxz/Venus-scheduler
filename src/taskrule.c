#include "task.h"

/*
 * Note: The following data is simulated
 *      1. SPMx_ADDR  ---> we should know the mapping address of every block's
 * spm --- <saddr.h>
 *      2. DATAx_ADDR ---> we should told DMA the destination of the data moving
 * to the block --- <daddr.h>
 *      3. DATA_LEN   ---> we should get the length of every data segment ---
 * <daddr.h>
 */

static fifo_t q_block;
extern void task1_exe(actor_t *g);
extern void task2_exe(actor_t *g);
extern void task3_exe(actor_t *g);

void task_bind(actor_t *g, block_f *n_block,
               Taskfunc task_addr) {
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

// Note: The allocating space should be catch somewhere, yet I don't know how to solve...
void alloc_result(actor_t *g){
  void *p;
  uint32_t i;
  for(i = 0;i<g->result_num[0];i++){
  p = malloc(32);
  uint32_t alloc_addr = (uint32_t)p;
    dma_result(alloc_addr, DATA1_ADDR, RESULT_DATALEN);
    printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
    put_data(g->out[0], alloc_addr, RESULT_DATALEN);
  }
 }

void task_exe(actor_t *g, block_f *n_block) {
  switch (g->task_addr){
  	case 1: 
  		task1_exe(g);
  		break;
  	case 2:
  		task2_exe(g);
  		break;
  	case 3:
  		task3_exe(g);
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

void task(actor_t *g) {
  // 1. check firing condition && catch idle block
  if (fifo_size(g->in[0]) >= g->dep_num[0] && fifo_size(&q_block) >= 1) {
    block_f *n_block = (block_f *)get_ptr(&q_block);
    printf("\nTASK2: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be busy status
    _set_block_flag(n_block, BLOCK_BUSY);
    // 3. pass the task code's revelent information to DMA
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    for(uint32_t i=0;i<g->dep_num[0];i++){
    data_t data = get_data(g->in[0]);
    dma_data(DATA1_ADDR, data.ptr, data.len);
    }
    // 4. associate block and task
    task_bind(g, n_block, &task_exe);
  }
}
