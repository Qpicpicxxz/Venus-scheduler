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

void task_bind(actorio_t *g_in, actorio_t *g_out, block_f *n_block,
               Taskfunc task_addr) {
  actorio_t *prev_actor = g_in;
  actorio_t *next_actor = g_out;
  printf("\nBLOCK: Computing task...\n");
  n_block->task_addr = (uint32_t)task_addr;
  n_block->prev_actor = prev_actor;
  n_block->next_actor = next_actor;
}

/*
 * block_recycle(&block1)
 * block_n: address of block_n flags
 * (block_f *)block_n: original pointer
 */
void block_recycle(block_f *block_n) {
  printf("enterning block_recycle...\n");
  // 1. check if this block needs to recycle result
  if (!_is_block_done(block_n)) {
    // 1.1 catch scheduler recycle handler's pointer
    Taskfunc prevtask = (Taskfunc)block_n->task_addr;
    // 1.2 call the handler with revelant actors
    prevtask(block_n->prev_actor, block_n->next_actor, block_n);
  }
  // 2. add current block into idle queue
  put_fifo(&q_block, (uint32_t)block_n);
}

// task1: in[0]  --> out[0]
void task1(actorio_t *g_in, actorio_t *g_out) {
  // check the firing condition && catch idle block
  if (fifo_size(g_in->in[0]) >= 1 && fifo_size(&q_block) >= 1) {
    block_f *n_block = (block_f *)get_fifo(&q_block);
    printf("\nTASK1: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    _set_block_flag(n_block, BLOCK_BUSY);
    dma_code(n_block->spm_addr, TASK1_START, TASK1_END - TASK1_START);
    dma_data(DATA1_ADDR, get_fifo(g_in->in[0]), DATA1_LEN);
    // bind current with selected block
    task_bind(g_in, g_out, n_block, &task1_exe);
  }
}

// task2: in[0] in[0] --> out[0]
void task2(actorio_t *g_in, actorio_t *g_out) {
  // 1. check firing condition && catch idle block
  if (fifo_size(g_in->in[0]) >= 2 && fifo_size(&q_block) >= 1) {
    block_f *n_block = (block_f *)get_fifo(&q_block);
    printf("\nTASK2: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be busy status
    _set_block_flag(n_block, BLOCK_BUSY);
    // 3. pass the task code's revelent information to DMA
    dma_code(n_block->spm_addr, TASK2_START, TASK2_END - TASK2_START);
    dma_data(DATA3_ADDR, get_fifo(g_in->in[0]), DATA3_LEN);
    dma_data(DATA4_ADDR, get_fifo(g_in->in[0]), DATA4_LEN);
    // 4. associate block and task
    task_bind(g_in, g_out, n_block, &task2_exe);
  }
}

// task3: in[0] in[0] --> out[0] out[0] out[0] out[0]
void task3(actorio_t *g_in, actorio_t *g_out) {
  if (fifo_size(g_in->in[0]) >= 2 && fifo_size(&q_block) >= 1) {
    block_f *n_block = (block_f *)get_fifo(&q_block);
    printf("\nTASK3: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    _set_block_flag(n_block, BLOCK_BUSY);
    dma_code(n_block->spm_addr, TASK3_START, TASK3_END - TASK3_START);
    // Problem: how we can allocate spm's memory to store data?
    dma_data(DATA3_ADDR, get_fifo(g_in->in[0]), DATA3_LEN);
    dma_data(DATA4_ADDR, get_fifo(g_in->in[0]), DATA4_LEN);
    task_bind(g_in, g_out, n_block, &task3_exe);
  }
}
