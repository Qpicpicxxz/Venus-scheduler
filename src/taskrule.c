#include "actor.h"

/* Note: The following data is simulated
        1. SPMx_ADDR  ---> we should know the mapping address of every block's
   spm --- <saddr.h>
        2. DATAx_ADDR ---> we should told DMA the destination of the data moving
   to the block --- <daddr.h>
        3. DATA_LEN   ---> we should get the length of every data segment ---
   <daddr.h>
        4. &datax_mem ---> we should allocate the memory in DDR for block's
   compute result
        5. block_task ---> we should have a handler to catch unoccupied blocks
   and schedule them
*/

static fifo_t q_block;

/*
  block_recycle(&block1)
  block_n: address of block_n flags
  (block_f *)block_n: original pointer
*/
void block_recycle(block_f *block_n) {
  printf("block addr: 0x%x\n", block_n);
  printf("prev task 0x%x\n", block_n->prev_actor_addr);
  printf("next task 0x%x\n", block_n->next_actor_addr);
  // 1. recycle previous task's result
  if (!_is_block_done(block_n)) {
    Taskfunc prevtask = (Taskfunc)(block_n->task_addr);
    prevtask((actorio_t *)(((block_f *)block_n)->prev_actor_addr),
             (actorio_t *)(((block_f *)block_n)->next_actor_addr),
             (uint32_t)block_n);
  }
  put_fifo(&q_block, (uint32_t)block_n);
}

// task1: in[0]  --> out[0]
void task1(actorio_t *g_in, actorio_t *g_out) {
  // check the firing condition && catch idle block
  if (fifo_size(g_in->in[0]) >= 1 && fifo_size(&q_block) >= 1) {
    int n_block = get_fifo(&q_block);
    printf("\nTASK1: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    _set_block_flag((block_f *)n_block, BLOCK_BUSY);
    dma_code(SPM1_ADDR, TASK1_START, TASK1_END - TASK1_START);
    dma_data(DATA1_ADDR, get_fifo(g_in->in[0]), DATA1_LEN);
    task1_bind(g_in, g_out, n_block);
  }
}

// task2: in[0] in[0] --> out[0]
void task2(actorio_t *g_in, actorio_t *g_out) {
  // 1. check firing condition && catch idle block
  if (fifo_size(g_in->in[0]) >= 2 && fifo_size(&q_block) >= 1) {
    int n_block = get_fifo(&q_block);
    printf("\nTASK2: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be busy status
    _set_block_flag((block_f *)n_block, BLOCK_BUSY);
    // 3. pass the task code's revelent information to DMA
    dma_code(SPM2_ADDR, TASK2_START, TASK2_END - TASK2_START);
    dma_data(DATA3_ADDR, get_fifo(g_in->in[0]), DATA3_LEN);
    dma_data(DATA4_ADDR, get_fifo(g_in->in[0]), DATA4_LEN);
    // 4. associate block and task
    task2_bind(g_in, g_out, n_block);
  }
}

// task3: in[0] in[0] --> out[0] out[0] out[0] out[0]
void task3(actorio_t *g_in, actorio_t *g_out) {
  if (fifo_size(g_in->in[0]) >= 2 && fifo_size(&q_block) >= 1) {
    int n_block = get_fifo(&q_block);
    printf("\nTASK3: Firing...\n");
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    _set_block_flag((block_f *)n_block, BLOCK_BUSY);
    dma_code(SPM3_ADDR, TASK3_START, TASK3_END - TASK3_START);
    dma_data(DATA3_ADDR, get_fifo(g_in->in[0]), DATA3_LEN);
    dma_data(DATA4_ADDR, get_fifo(g_in->in[0]), DATA4_LEN);
    task3_bind(g_in, g_out, n_block);
  }
}
