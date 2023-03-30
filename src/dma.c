#include "task.h"
#include "dma.h"
/*
 * Use DMA to move the code to the designated place
 * i_spm_addr: the target block's code memory address (destination)
 * task_addr: the origin task's address in DDR (source)
 * task_len: the total length of this task (length)
*/

void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len) {
  // delay(5000);
  printf("DMA: Received task address 0x%x\n", task_addr);
  lli_t* current_lli   = malloc_LLI();
  uint32_t total_chunk = (task_len / DMA_MAX_TRANSFER_LENGTH) + 1;
  for (int i = 0; i < total_chunk; i++) {
    // if its the last chunk to be transmitted
    if (i == total_chunk - 1) {
      lli_setup(i_spm_addr,
                task_addr,
                task_len,
                current_lli,
                current_lli,
                LAST_SHADOW_REGISTER);
    } else {
      lli_t* next_lli = malloc_LLI();
      lli_setup(i_spm_addr,
                task_addr,
                DMA_MAX_TRANSFER_LENGTH,
                current_lli,
                next_lli,
                NOT_LAST_SHADOW_REGISTER);
      task_addr   = task_addr + 4 * (DMA_MAX_TRANSFER_LENGTH);
      task_len    = task_len - DMA_MAX_TRANSFER_LENGTH;
      current_lli = next_lli;
    }
  }
  printf("Reserved0 %lx", current_lli->Reserved0);
  printf("CHx_LLP_STATUS %lx", current_lli->CHx_LLP_STATUS);
  printf("WB_CHx_DSTAT %x", current_lli->WB_CHx_DSTAT);
  printf("WB_CHx_SSTAT %x", current_lli->WB_CHx_SSTAT);
  printf("CHx_CTL %lx", current_lli->CHx_CTL);
  printf("CHx_LLP %lx", current_lli->CHx_LLP);
  printf("Reserved1 %x", current_lli->Reserved1);
  printf("CHx_BLOCK_TS %x", current_lli->CHx_BLOCK_TS);
  printf("CHx_DAR %lx", current_lli->CHx_DAR);
  printf("CHx_SAR %lx", current_lli->CHx_SAR);
}

void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len) {
  delay(5000);
  printf("DMA: Received data address 0x%x\n", data_addr);
}

void dma_result(uint32_t data_dst, uint32_t data_addr, uint32_t data_len) {
  delay(5000);
}

void dma_test(void) {
  uint32_t i_spm_addr = 0x99999999;
  uint32_t task_addr  = 0x33333333;
  uint32_t task_len   = 4;
  dma_code(i_spm_addr, task_addr, task_len);
}

