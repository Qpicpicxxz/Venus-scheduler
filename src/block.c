#include "task.h"

/* A global variable for result data simulate */
uint32_t result;

void task1_exe(actor_t *g) {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  result = t1 * t1;
  printf("TASK1: in1 * in1 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
}

void task2_exe(actor_t *g) {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  result = t1 + t1;
  printf("TASK2: in1 + in1 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
}

void task3_exe(actor_t *g) {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  data_t *data2 = read_else_data(&dma_trans_in, 1);
  uint32_t t2 = *(uint32_t *)data2->ptr;
  result = t1 + t2;
  printf("TASK3: in1 + in2 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
}

void task4_exe(actor_t *g) {
  data_t *data1 = read_data(&dma_trans_in);
  data_t *data2 = read_else_data(&dma_trans_in, 1);
  uint32_t t1 = *(uint32_t *)data1->ptr;
  uint32_t t2 = *(uint32_t *)data2->ptr;
  result = t1 * t2;
  printf("TASK4: in1 * in2 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
}

void task5_exe(actor_t *g) {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  result = t1 * t1;
  printf("TASK5: in1 * in1 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
}

void task6_exe(actor_t *g) {
  data_t *data1 = read_data(&dma_trans_in);
  data_t *data2 = read_else_data(&dma_trans_in, 1);
  data_t *data3 = read_else_data(&dma_trans_in, 2);
  uint32_t  t1 = *(uint32_t *)data1->ptr;
  uint32_t  t2 = *(uint32_t *)data2->ptr;
  uint32_t  t3 = *(uint32_t *)data3->ptr;
  result = t1 + t2 + t3;
  printf("TASK6: in1 + in2 + in3 = out = %d\n", result);
  printf("\nBLOCK: Job done...\n");
}
