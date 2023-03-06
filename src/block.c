#include "task.h"

void task1_exe(actor_t *g) {
  data_t data = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data.ptr;
  uint32_t result = *t1 * *t1;
  free((void *)t1);
  printf("TASK1: in1 * in1 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, g->result_len);
}

void task2_exe(actor_t *g) {
  data_t data = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data.ptr;
  uint32_t result = *t1 + *t1;
  free((void *)t1);
  printf("TASK2: in1 + in1 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, g->result_len);
}

void task3_exe(actor_t *g) {
  data_t data1 = get_data(&dma_trans_in);
  data_t data2 = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data1.ptr;
  uint32_t *t2 = (uint32_t *)data2.ptr;
  int result = *t1 + *t2;
  free((void *)t1);
  free((void *)t2);
  printf("TASK3: in1 - in2 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, g->result_len);
}

void task4_exe(actor_t *g) {
  data_t data1 = get_data(&dma_trans_in);
  data_t data2 = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data1.ptr;
  uint32_t *t2 = (uint32_t *)data2.ptr;
  int result = *t1 * *t2;
  free((void *)t1);
  free((void *)t2);
  printf("TASK4: in1 * in2 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, g->result_len);
}

void task5_exe(actor_t *g) {
  data_t data = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data.ptr;
  uint32_t result = *t1 * *t1;
  free((void *)t1);
  printf("TASK5: in1 * in1 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, g->result_len);
}

void task6_exe(actor_t *g) {
  data_t data1 = get_data(&dma_trans_in);
  data_t data2 = get_data(&dma_trans_in);
  data_t data3 = get_data(&dma_trans_in);
  uint32_t *t1 = (uint32_t *)data1.ptr;
  uint32_t *t2 = (uint32_t *)data2.ptr;
  uint32_t *t3 = (uint32_t *)data3.ptr;
  int result = *t1 + *t2 + *t3;
  free((void *)t1);
  free((void *)t2);
  free((void *)t3);
  printf("TASK6: in1 + in2 + in3 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
  put_data(&dma_trans_out, result, g->result_len);
}
