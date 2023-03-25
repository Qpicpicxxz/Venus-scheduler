/* 
 * Note: we have to ensure that this file always do nothing!!! 
 * JUST simulate DMA's behaviour!!!
 * DO NOT put any practical operation of data in it!!!
 */
#include "task.h"
/*
 * Use DMA to move the code to the designated place
 * i_spm_addr: the target block's code memory address (destination)
 * task_addr: the origin task's address in DDR (source)
 * task_len: the total length of this task (length)
 */
void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len) {
  delay(5000);
  printf("\n");
  // printf("DMA: Received i_spm_addr 0x%x\n", i_spm_addr);
  // printf("DMA: Received task_addr 0x%x\n", task_addr);
  // printf("DMA: Received task_len 0x%x\n", task_len);
  printf("DMA: Received task address 0x%x\n", task_addr);
}

/* 
 * Use DMA to move the data to the designated place
 * data_dst: the target block's data memory address (destination)
 * data_addr: the original data address in DDR (source)
 * data_len: the length of the data (length)
 */
void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len) {
  delay(5000);
  // dma get the data address
  // printf("DMA: Received data_dst 0x%x\n", data_dst);
  // printf("DMA: Received data_addr 0x%x\n", data_addr);
  // printf("DMA: Received data_len 0x%x\n", data_len);
  printf("DMA: Received data address 0x%x\n", data_addr);
}

/*
 * Use DMA to transmit the result back to the allocated place
 * data_dst: the allocated memory to store the result in DDR (destination)
 * data_addr: the result address in block's memory (source)
 * data_len: the length of the result (length)
 */
void dma_result(uint32_t data_dst, uint32_t data_addr, uint32_t data_len){
  delay(5000);
  printf("DMA: Reterned data result from 0x%x\n", data_dst);
 }
 
