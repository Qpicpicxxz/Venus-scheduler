/* Simulate block's behaviour, DO NOT exist in real scheme. */

#include "task.h"
// static block_t* block;
extern uint8_t actor_index;

/*
 * For test:
 * 	1. Simulate block process and pass the data to the task
 *	2. Store data descriptor in it to simulate
 */
fifo_t dma_trans_in;

// void taskA_exe() {
//   data_t* data = read_data(&dma_trans_in);
//   uint32_t t1 = *(uint32_t*)data->ptr;
//   block->result = t1;
// }

// void taskB_exe() {
//   data_t* data = read_data(&dma_trans_in);
//   uint32_t t1 = *(uint32_t*)data->ptr;
//   block->result = t1 + t1;
// }

// void taskC_exe() {
//   data_t* data = read_data(&dma_trans_in);
//   uint32_t t1 = *(uint32_t*)data->ptr;
//   block->result = t1 * t1;
// }

// void taskD_exe() {
//   data_t* data = read_data(&dma_trans_in);
//   uint32_t t1 = *(uint32_t*)data->ptr;
//   data_t* data2 = read_else_data(&dma_trans_in, 1);
//   uint32_t t2 = *(uint32_t*)data2->ptr;
//   data_t* data3 = read_else_data(&dma_trans_in, 2);
//   uint32_t t3 = *(uint32_t*)data3->ptr;
//   block->result = t1 + t2 + t3;
// }

// void taskE_exe() {
//   data_t* data = read_data(&dma_trans_in);
//   uint32_t t1 = *(uint32_t*)data->ptr;
//   block->result = t1 + t1;
// }

// void taskF_exe() {
//   data_t* data = read_data(&dma_trans_in);
//   uint32_t t1 = *(uint32_t*)data->ptr;
//   block->result = t1 * t1;
// }

// void taskG_exe() {
//   data_t* data = read_data(&dma_trans_in);
//   uint32_t t1 = *(uint32_t*)data->ptr;
//   data_t* data2 = read_else_data(&dma_trans_in, 1);
//   uint32_t t2 = *(uint32_t*)data2->ptr;
//   data_t* data3 = read_else_data(&dma_trans_in, 2);
//   uint32_t t3 = *(uint32_t*)data3->ptr;
//   block->result = t1 + t2 + t3;
// }

// void task_exe() {
//   block->result = 1;
// }

void block_sim(block_t* n_block) {
  printf(WHITE("actor %c is computing...\n"), actor_index + 65);
  n_block->result = 1;
}

// void block_sim(block_t* n_block) {
//   block = n_block;
//   switch (actor_index) {
//   case 0:
//     taskA_exe();
//     break;
//   case 1:
//     taskB_exe();
//     break;
//   case 2:
//     taskC_exe();
//     break;
//   case 3:
//     taskD_exe();
//     break;
//   case 4:
//     taskE_exe();
//     break;
//   case 5:
//     taskF_exe();
//     break;
//   case 6:
//     taskG_exe();
//     break;
//   default:
//     printf("\nERROR: No task code matching...\n");
//     break;
//   }
// }
