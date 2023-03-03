#include "task.h"

fifo_t dma_trans;

// dma should move the code to the designated place
void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len) {
  task_delay(5000);
  printf("\n");
  // printf("DMA: Received i_spm_addr 0x%x\n", i_spm_addr);
  // printf("DMA: Received task_addr 0x%x\n", task_addr);
  // printf("DMA: Received task_len 0x%x\n", task_len);
  printf("DMA: Received task address 0x%x\n", task_addr);
}

// dma should move the data to the designated place
void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len) {
  task_delay(5000);
  // dma get the data address
  put_fifo(&dma_trans, data_addr);
  // printf("DMA: Received data_dst 0x%x\n", data_dst);
  // printf("DMA: Received data_addr 0x%x\n", data_addr);
  // printf("DMA: Received data_len 0x%x\n", data_len);
  printf("DMA: Received data address 0x%x\n", data_addr);
}
