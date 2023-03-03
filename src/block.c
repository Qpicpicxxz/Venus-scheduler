#include "task.h"

void task1_exe(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  printf("\nBLOCK: Job done...\n");
  //((block_f *)n_block)->recycle_addr
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans);
  int result = *t1 * *t1;
  // block will return the data result, hence scheduler should have to allocatez
  // the address for the result Note: I have no idea how to get the data size
  printf("\nSCHEDULER: Allocating result...\n");
  // Note: the minimum allocate unit is 256, so malloc(1) and malloc(256) have
  // the same effect
  void *p = malloc(32);
  printf("SCHEDULER: Result is stored in: 0x%x\n", p);
  *(int *)p = result;
  put_fifo(g_in->out[0], (uint32_t)p);
  printf("in * in = out = %d\n", *(int *)read_fifo(g_in->out[0]));
  // pass the result data pointer to the successor
  put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  _clear_block_flag(n_block);
  printf("SCHEDULER: The result data has been recycled...\n");
}

void task2_exe(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
   printf("\nBLOCK: Job done...\n");
  // simulate block's computing process...
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans);
  uint32_t *t2 = (uint32_t *)get_fifo(&dma_trans);
  int result = *t1 + *t2;
  printf("\nSCHEDULER: Allocating result...\n");
  // allocate some heap memory space for result
  void *p = malloc(32);
  printf("SCHEDULER: Result is stored in: 0x%x\n", p);
  *(int *)p = result;
  // put the result data into current actor's output fifo
  put_fifo(g_in->out[0], (uint32_t)p);
  printf("in1 + in2 = out = %d\n", *(int *)read_fifo(g_in->out[0]));
  // put the data to the successor
  put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  // mark the block back to idle
  _clear_block_flag(n_block);
  printf("SCHEDULER: The result data has been recycled...\n");
}

void task3_exe(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  printf("\nBLOCK: Job done...\n");
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans);
  uint32_t *t2 = (uint32_t *)get_fifo(&dma_trans);
  int result_1 = *t1 + *t2;
  int result_2 = *t1 - *t2;
  int result_3 = *t1 * *t2;
  int result_4 = *t1 / *t2;
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(32);
  *(int *)p = result_1;
  printf("SCHEDULER: Result is stored in 0x%x\n", p);
  put_fifo(g_in->out[0], (uint32_t)p);
  p = malloc(32);
  *(int *)p = result_2;
  printf("SCHEDULER: Result is stored in 0x%x\n", p);
  put_fifo(g_in->out[0], (uint32_t)p);
  p = malloc(32);
  *(int *)p = result_3;
  printf("SCHEDULER: Result is stored in 0x%x\n", p);
  put_fifo(g_in->out[0], (uint32_t)p);
  p = malloc(32);
  *(int *)p = result_4;
  printf("SCHEDULER: Result is stored in 0x%x\n", p);
  put_fifo(g_in->out[0], (uint32_t)p);
  printf("in1 + in2 = out = %d\n", *(uint32_t *)read_fifo(g_in->out[0]));
  printf("in1 - in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 1));
  printf("in1 * in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 2));
  printf("in1 / in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 3));
  put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  _clear_block_flag(n_block);
  printf("SCHEDULER: The result data has been recycled...\n");
}

void task1_bind(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  Taskfunc task_addr = &task1_exe;
  actorio_t *prev_actor = g_in;
  actorio_t *next_actor = g_out;
  printf("\nBLOCK: Computing task1...\n");
  n_block->task_addr = (uint32_t)task_addr;
  n_block->prev_actor = prev_actor;
  n_block->next_actor = next_actor;
}

void task2_bind(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  Taskfunc task_addr = &task2_exe;
  actorio_t *prev_actor = g_in;
  actorio_t *next_actor = g_out;
  printf("\nBLOCK: Computing task2...\n");
  n_block->task_addr = (uint32_t)task_addr;
  n_block->prev_actor = prev_actor;
  n_block->next_actor = next_actor;
}

void task3_bind(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  Taskfunc task_addr = &task3_exe;
  actorio_t *prev_actor = g_in;
  actorio_t *next_actor = g_out;
  printf("\nBLOCK: Computing task3...\n");
  n_block->task_addr = (uint32_t)task_addr;
  n_block->prev_actor = prev_actor;
  n_block->next_actor = next_actor;
}
