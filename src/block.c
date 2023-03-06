#include "task.h"

void task1_exe(actor_t *g) {
  data_t data = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data.ptr;
  uint32_t result = *t1 * *t1;
  free((void *)t1);
  printf("in1 * in1 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  // Note: I haven't extinguish dependency data length and result data length...
  put_data(&dma_trans_out, result, RESULT_DATALEN);
}

void task2_exe(actor_t *g) {
  data_t data1 = get_data(&dma_trans_in);
  data_t data2 = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data1.ptr;
  uint32_t *t2 = (uint32_t *)data2.ptr;
  int result = *t1 + *t2;
  free((void *)t1);
  free((void *)t2);
  printf("in1 + in2 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, RESULT_DATALEN);
}

void task3_exe(actor_t *g) {
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
  printf("in1 + in2 = out = %d\n", result_1);
  printf("in1 - in2 = out = %d\n", result_2);
  printf("in1 * in2 = out = %d\n", result_3);
  printf("in1 / in2 = out = %d\n", result_4);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result_1, RESULT_DATALEN);
  put_data(&dma_trans_out, result_2, RESULT_DATALEN);
  put_data(&dma_trans_out, result_3, RESULT_DATALEN);
  put_data(&dma_trans_out, result_4, RESULT_DATALEN);
}
