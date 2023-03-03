#include "task.h"

void task1_exe(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  // 1. simulate block's computation process
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans_in);
  uint32_t result = *t1 * *t1;
  printf("\nBLOCK: Job done...\n");
  put_fifo(&dma_trans_out, result);
  printf("\nSCHEDULER: Allocating result...\n");
  /*
   * Note: the minimum allocate unit is 256, so malloc(1) and malloc(256) have
   * 2. Based on the result data's length
   * scheduler allocate the appropriate memory
   */
  void *p = malloc(32);
  uint32_t alloc_addr = (uint32_t)p;
  // 3. scheduler inform dma to transmit the result back
  dma_result(DATA1_ADDR, alloc_addr, DATA1_LEN);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  // 4. reset the block's flag
  _clear_block_flag(n_block);
  // 5. pass the result to the actor's output fifo
  put_fifo(g_in->out[0], alloc_addr);
  printf("in * in = out = %d\n", *(int *)read_fifo(g_in->out[0]));
  // 6. pass the output fifo to the successor's input fifo
  put_fifo(g_out->in[0], get_fifo(g_in->out[0]));
  printf("SCHEDULER: The result data has been recycled...\n");
}

void task2_exe(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans_in);
  uint32_t *t2 = (uint32_t *)get_fifo(&dma_trans_in);
  int result = *t1 + *t2;
  printf("\nBLOCK: Job done...\n");
  put_fifo(&dma_trans_out, result);
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(32);
  uint32_t alloc_addr = (uint32_t)p;
  dma_result(DATA2_ADDR, alloc_addr, DATA2_LEN);
  printf("SCHEDULER: Result is stored in: 0x%x\n", alloc_addr);
  _clear_block_flag(n_block);
  put_fifo(g_in->out[0], alloc_addr);
  printf("in1 + in2 = out = %d\n", *(int *)read_fifo(g_in->out[0]));
  put_fifo(g_out->in[0], get_fifo(g_in->out[0]));
  printf("SCHEDULER: The result data has been recycled...\n");
}

void task3_exe(actorio_t *g_in, actorio_t *g_out, block_f *n_block) {
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans_in);
  uint32_t *t2 = (uint32_t *)get_fifo(&dma_trans_in);
  int result_1 = *t1 + *t2;
  int result_2 = *t1 - *t2;
  int result_3 = *t1 * *t2;
  int result_4 = *t1 / *t2;
  printf("\nBLOCK: Job done...\n");
  put_fifo(&dma_trans_out, result_1);
  put_fifo(&dma_trans_out, result_2);
  put_fifo(&dma_trans_out, result_3);
  put_fifo(&dma_trans_out, result_4);
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(32);
  uint32_t alloc_addr = (uint32_t)p;
  dma_result(DATA1_ADDR, alloc_addr, DATA1_LEN);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_fifo(g_in->out[0], alloc_addr);
  p = malloc(32);
  alloc_addr = (uint32_t)p;
  dma_result(DATA1_ADDR, alloc_addr, DATA1_LEN);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_fifo(g_in->out[0], alloc_addr);
  p = malloc(32);
  alloc_addr = (uint32_t)p;
  dma_result(DATA1_ADDR, alloc_addr, DATA1_LEN);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_fifo(g_in->out[0], alloc_addr);
  p = malloc(32);
  alloc_addr = (uint32_t)p;
  dma_result(DATA1_ADDR, alloc_addr, DATA1_LEN);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_fifo(g_in->out[0], alloc_addr);
  printf("in1 + in2 = out = %d\n", *(uint32_t *)read_fifo(g_in->out[0]));
  printf("in1 - in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 1));
  printf("in1 * in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 2));
  printf("in1 / in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 3));
  for (int i = 0; i < 4; i++) {
     put_fifo(g_out->in[0], get_fifo(g_in->out[0]));
  }
  _clear_block_flag(n_block);
  printf("SCHEDULER: The result data has been recycled...\n");
}
