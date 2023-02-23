#include "actor.h"

// for test, simulate block process and pass the data to the task
static fifo_t dma_trans;

// dma should move the code to the designated place
void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len) {
  task_delay(5000);
  printf("\n");
  printf("DMA: Received i_spm_addr 0x%x\n", i_spm_addr);
  printf("DMA: Received task_addr 0x%x\n", task_addr);
  printf("DMA: Received task_len 0x%x\n", task_len);
}

// dma should move the data to the designated place
void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len) {
  task_delay(5000);
  printf("\n");
  // dma get the data address
  put_fifo(&dma_trans, data_addr);
  printf("DMA: Received data_dst 0x%x\n", data_dst);
  printf("DMA: Received data_addr 0x%x\n", data_addr);
  printf("DMA: Received data_len 0x%x\n", data_len);
}

void block_task1(actorio_t *g) {
  printf("\nBLOCK: Computing task1...\n");
  task_delay(10000);
  // simulate the data transmit from DMA
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans);
  int result = *t1 * *t1;
  // block will return the data result, hence scheduler should have to allocate
  // the address for the result Note: I have no idea how to get the data size
  printf("\nSCHEDULER: Allocating result...\n");
  // Note: the minimum allocate unit is 256, so malloc(1) and malloc(256) have the same effect
  void *p = malloc(32);
  // *(int *)p point to the result data
  *(int *)p = result;
  printf("The result data is stored in 0x%x\n", p);
  put_fifo(g->out[0], (uint32_t)p);
}

void block_task2(actorio_t *g) {
  printf("\nBLOCK: Computing task2...\n");
  task_delay(10000);
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans);
  uint32_t *t2 = (uint32_t *)get_fifo(&dma_trans);
  int result = *t1 + *t2;
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(32);
  *(int *)p = result;
  printf("The result data is stored in 0x%x\n", p);
  put_fifo(g->out[0], (uint32_t)p);
}

void block_task3(actorio_t *g) {
  printf("\nBLOCK: Computing task3...\n");
  task_delay(10000);
  uint32_t *t1 = (uint32_t *)get_fifo(&dma_trans);
  uint32_t *t2 = (uint32_t *)get_fifo(&dma_trans);
  int result_1 = *t1 + *t2;
  int result_2 = *t1 - *t2;
  int result_3 = *t1 * *t2;
  int result_4 = *t1 / *t2;
  printf("\nSCHEDULER: Allocating result...\n");
  void *p = malloc(32);
  *(int *)p = result_1;
  printf("The result data is stored in 0x%x\n", p);
  put_fifo(g->out[0], (uint32_t)p);
  p = malloc(32);
  *(int *)p = result_2;
  printf("The result data is stored in 0x%x\n", p);
  put_fifo(g->out[0], (uint32_t)p);
  p = malloc(32);
  *(int *)p = result_3;
  printf("The result data is stored in 0x%x\n", p);
  put_fifo(g->out[0], (uint32_t)p);
  p = malloc(32);
  *(int *)p = result_4;
  printf("The result data is stored in 0x%x\n", p);
  put_fifo(g->out[0], (uint32_t)p);
}
