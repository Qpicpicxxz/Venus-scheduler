#include "task.h"

void task1_exe(actor_t *g_in, actor_t *g_out, block_f *n_block) {

  // 1. simulate block's computation process
  data_t data = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data.ptr;
  uint32_t result = *t1 * *t1;
  // 2. free data space (under practical circumstances we can free the data address right after DMA transmitted the data into block's spm)
  free((void *)t1);
  printf("\nBLOCK: Job done...\n");
  // Note: I haven't extinguish dependency data length and result data length...
  put_data(&dma_trans_out, result, data.len);
  /*
   * Note: the minimum allocate unit is 256, so malloc(1) and malloc(256) have
   * 3. Based on the result data's length
   * scheduler allocate the appropriate memory
   */
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(data.len);
  uint32_t alloc_addr = (uint32_t)p;
  // 4. scheduler inform dma to transmit the result back
  dma_result(alloc_addr, DATA1_ADDR, data.len);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  
  // 5. reset the block's flag
  _clear_block_flag(n_block);
  
  // 6. pass the result to the actor's output fifo
  put_data(g_in->out[0], alloc_addr, data.len);
  printf("in * in = out = %d\n", *(int *)read_fifo(g_in->out[0]));
  data = get_data(g_in->out[0]);
  // 7. pass the output fifo to the successor's input fifo
  put_data(g_out->in[0], data.ptr, data.len);
  printf("SCHEDULER: The result data has been recycled...\n");
}

void task2_exe(actor_t *g_in, actor_t *g_out, block_f *n_block) {
  data_t data1 = get_data(&dma_trans_in);
  data_t data2 = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data1.ptr;
  uint32_t *t2 = (uint32_t *)data2.ptr;
  int result = *t1 + *t2;
  free((void *)t1);
  free((void *)t2);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, data1.len);
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(32);
  uint32_t alloc_addr = (uint32_t)p;
  dma_result(alloc_addr, DATA1_ADDR, data1.len);
  printf("SCHEDULER: Result is stored in: 0x%x\n", alloc_addr);
  _clear_block_flag(n_block);
  put_data(g_in->out[0], alloc_addr, data1.len);
  printf("in1 + in2 = out = %d\n", *(int *)read_fifo(g_in->out[0]));
  data_t data = get_data(g_in->out[0]);
  put_data(g_out->in[0], data.ptr, data1.len);
  printf("SCHEDULER: The result data has been recycled...\n");
}

void task3_exe(actor_t *g_in, actor_t *g_out, block_f *n_block) {
  data_t data1 = get_data(&dma_trans_in);
  data_t data2 = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data1.ptr;
  uint32_t *t2 = (uint32_t *)data2.ptr;
  int result_1 = *t1 + *t2;
  int result_2 = *t1 - *t2;
  int result_3 = *t1 * *t2;
  int result_4 = *t1 / *t2;
  free((void *)t1);
  free((void *)t2);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result_1, data1.len);
  put_data(&dma_trans_out, result_2, data1.len);
  put_data(&dma_trans_out, result_3, data1.len);
  put_data(&dma_trans_out, result_4, data1.len);
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(32);
  uint32_t alloc_addr = (uint32_t)p;
  dma_result(alloc_addr, DATA1_ADDR, data1.len);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_data(g_in->out[0], alloc_addr, data1.len);
  p = malloc(32);
  alloc_addr = (uint32_t)p;
  dma_result(alloc_addr, DATA1_ADDR, data1.len);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_data(g_in->out[0], alloc_addr, data1.len);
  p = malloc(32);
  alloc_addr = (uint32_t)p;
  dma_result(alloc_addr, DATA1_ADDR, data1.len);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_data(g_in->out[0], alloc_addr, data1.len);
  p = malloc(32);
  alloc_addr = (uint32_t)p;
  dma_result(alloc_addr, DATA1_ADDR, data1.len);
  printf("SCHEDULER: Result is stored in 0x%x\n", alloc_addr);
  put_data(g_in->out[0], alloc_addr, data1.len);
  printf("in1 + in2 = out = %d\n", *(uint32_t *)read_fifo(g_in->out[0]));
  printf("in1 - in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 1));
  printf("in1 * in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 2));
  printf("in1 / in2 = out = %d\n",
         *(uint32_t *)read_else_fifo(g_in->out[0], 3));
  for (int i = 0; i < 4; i++) {
     data_t data = get_data(g_in->out[0]);
     put_data(g_out->in[0], data.ptr, data.len);
  }
  _clear_block_flag(n_block);
  printf("SCHEDULER: The result data has been recycled...\n");
}
