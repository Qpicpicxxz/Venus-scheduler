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

// task1: in[0]  --> out[0]
void task1(actor_t *g) {
  // check the firing condition && catch idle block
  if (fifo_size(g->in[0]) >= g->dep_num[0] && fifo_size(&q_block) >= 1) {
    block_f *n_block = (block_f *)get_ptr(&q_block);
    printf("\nTASK1: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    _set_block_flag(n_block, BLOCK_BUSY);
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    data_t data = get_data(g->in[0]);
    dma_data(DATA1_ADDR, data.ptr, data.len);
    // bind current with selected block
    task_bind(g, n_block, &task1_exe);
  }
}

// task2: in[0] in[0] --> out[0]
void task2(actor_t *g) {
  // 1. check firing condition && catch idle block
  if (fifo_size(g->in[0]) >= g->dep_num[0] && fifo_size(&q_block) >= 1) {
    block_f *n_block = (block_f *)get_ptr(&q_block);
    printf("\nTASK2: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be busy status
    _set_block_flag(n_block, BLOCK_BUSY);
    // 3. pass the task code's revelent information to DMA
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    data_t data = get_data(g->in[0]);
    dma_data(DATA3_ADDR, data.ptr, data.len);
    data = get_data(g->in[0]);
    dma_data(DATA4_ADDR, data.ptr, data.len);
    // 4. associate block and task
    task_bind(g, n_block, &task2_exe);
  }
}

// task3: in[0] in[0] --> out[0] out[0] out[0] out[0]
void task3(actor_t *g) {
  if (fifo_size(g->in[0]) >= g->dep_num[0] && fifo_size(&q_block) >= 1) {
    block_f *n_block = (block_f *)get_ptr(&q_block);
    printf("\nTASK3: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    _set_block_flag(n_block, BLOCK_BUSY);
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    // Problem: how we can allocate spm's memory to store data?
    data_t data = get_data(g->in[0]);
    dma_data(DATA3_ADDR, data.ptr, data.len);
    data = get_data(g->in[0]);
    dma_data(DATA4_ADDR, data.ptr, data.len);
    // 4. associate block and task
    task_bind(g, n_block, &task3_exe);
  }
}
