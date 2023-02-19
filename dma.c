#include "actor.h"

// for test, simulate block process and pass the data to the task
static fifo_t d1;

// dma should move the code to the designated place
void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len)
{
  task_delay(5000);
  printf("\n");
  printf("DMA: Received i_spm_addr 0x%x\n", i_spm_addr);
  printf("DMA: Received task_addr 0x%x\n", task_addr);
  printf("DMA: Received task_len 0x%x\n", task_len);
}

// dma should move the data to the designated place
void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len)
{
  task_delay(5000);
  printf("\n");
  put_fifo(&d1, data_addr);
  printf("DMA: Received data_dst 0x%x\n", data_dst);
  printf("DMA: Received data_addr 0x%x\n", data_addr);
  printf("DMA: Received data_len 0x%x\n", data_len);
}

void block_task1(actorio_t *g)
{
  printf("\nBLOCK: Computing task1...\n");
  task_delay(10000);
  uint32_t *t1 = (uint32_t *)get_fifo(&d1);
  uint32_t *t2 = (uint32_t *)get_fifo(&d1);
  put_fifo(g->out[0], *t1 + *t2);
  put_fifo(g->out[0], *t1 - *t2);
}

void block_task2(actorio_t *g)
{
  printf("\nBLOCK: Computing task2...\n");
  task_delay(10000);
  uint32_t *t1 = (uint32_t *)get_fifo(&d1);
  uint32_t *t2 = (uint32_t *)get_fifo(&d1);
  put_fifo(g->out[0], *t1 + *t1);
  put_fifo(g->out[1], *t2 + *t2);
}
