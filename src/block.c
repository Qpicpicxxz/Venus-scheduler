#include "task.h"

static block_f *block;

void task1_exe() {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  block->result = t1 * t1;
}

void task2_exe() {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  block->result = t1 + t1;
}

void task3_exe() {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  data_t *data2 = read_else_data(&dma_trans_in, 1);
  uint32_t t2 = *(uint32_t *)data2->ptr;
  block->result = t1 + t2;
}

void task4_exe() {
  data_t *data1 = read_data(&dma_trans_in);
  data_t *data2 = read_else_data(&dma_trans_in, 1);
  uint32_t t1 = *(uint32_t *)data1->ptr;
  uint32_t t2 = *(uint32_t *)data2->ptr;
  block->result = t1 * t2;
}

void task5_exe() {
  data_t *data = read_data(&dma_trans_in);
  uint32_t t1 = *(uint32_t *)data->ptr;
  block->result = t1 * t1;
}

void task6_exe() {
  data_t *data1 = read_data(&dma_trans_in);
  data_t *data2 = read_else_data(&dma_trans_in, 1);
  data_t *data3 = read_else_data(&dma_trans_in, 2);
  uint32_t  t1 = *(uint32_t *)data1->ptr;
  uint32_t  t2 = *(uint32_t *)data2->ptr;
  uint32_t  t3 = *(uint32_t *)data3->ptr;
  block->result = t1 + t2 + t3;
}

/*
 * Function:
 *	Simulate block's behaviour, DO NOT exist in real scheme.
 */
void block_sim(uint32_t task_index, block_f *n_block) {
  block = n_block;
  switch (task_index) {
  case 1:
    task1_exe();
    break;
  case 2:
    task2_exe();
    break;
  case 3:
    task3_exe();
    break;
  case 4:
    task4_exe();
    break;
  case 5:
    task5_exe();
    break;
  case 6:
    task6_exe();
    break;
  default:
    printf("\nERROR: No task code matching...\n");
    break;
  }
}
