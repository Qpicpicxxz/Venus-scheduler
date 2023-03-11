#include "task.h"
#define NUM_TASKS 6

static actor_t task1_io;
static actor_t task2_io;
static actor_t task3_io;
static actor_t task4_io;
static actor_t task5_io;
static actor_t task6_io;
actor_t *task_io[NUM_TASKS] = {&task1_io, &task2_io, &task3_io, &task4_io, &task5_io, &task6_io};
static fifo_t q1, q2, q3, q4, q5, q6, q7, q8, q9, q10, q11;

static queue_t q_block;

/*
 * Generate actors: taskx_io
 * Describe a task:
 * 	1. dependent token: a specific fifo {&qx}
 *	2. how many token needed in every dep_task's io: dep_num
 *	3. how many result would be produced: result_num
 *	4. where are the results going to go: a specific fifo {&qx}
 */
void actor_create(void) {
  /*
   * Describe example:
   * 	1. {&q1}: input fifo array (curly brace represent an array)
   *	2. 1: dependency number
   *	3. &q3: output fifo
   *	4. 32: result data length
   *	5. 1: task start address (so far, simulate)
   *	6. TASK1_END - TAKS1_START: task length
   */
  *task_io[0] = (actor_t){{&q1},   
                       1,
                       {&q3, &q4},
                       2,
                       32,   
                       1, 	// Note: should be TASKn_START, NOT complete yet.I use it as a task token for simulation.
                       TASK1_END - TASK1_START};
  task2_io = (actor_t){{&q2}, 	// to represent dependency
                       1,
                       {&q5, &q6, &q7},
                       3,
                       32,
                       2,
                       TASK2_END - TASK2_START};
  task3_io = (actor_t){{&q3, &q5}, 
                       2, 
                       {&q8},
                       1,  
                       32,
                       3,
                       TASK3_END - TASK3_START};
  task4_io = (actor_t){{&q4, &q6}, 
                       2, 
                       {&q9},
                       1, 
                       32,
                       4,
                       TASK3_END - TASK3_START};
  task5_io = (actor_t){{&q7}, 
                       1, 
                       {&q10},  
                       1,
                       32,
                       5,
                       TASK3_END - TASK3_START};
  task6_io = (actor_t){{&q8, &q9, &q10}, 
                       3, 
                       {&q11},
                       1, 
                       32,
                       6,
                       TASK3_END - TASK3_START};
}

/* inject original stimulators */
void stimu_inject(void) {
  void *p;
  uint32_t data_len = 32;
  for (int i = 1; i > 0; i--){
  	// 1.1 alloc data memory
  	p = malloc(data_len);
  	// 1.2 initialize data
  	*(int *)p = 10;
  	uint32_t alloc_addr = (uint32_t)p;
  	// 2.1. alloc data descriptor's memory
  	p = malloc(1);
  	// 2.2 initialize data descriptor
  	data_t* data = (data_t *)p;
  	data->ptr = alloc_addr;
  	data->len = data_len;
  	data->cnt = 1;
  	// 3. put data descriptor's pointer into stimulate fifo
 	put_data(&q1, data);
  }
  for (int i = 1; i > 0; i--){
  	p = malloc(data_len);
  	*(int *)p = 9;
  	uint32_t alloc_addr = (uint32_t)p;
  	p = malloc(1);
  	data_t* data = (data_t *)p;
  	data->ptr = alloc_addr;
  	data->len = data_len;
  	data->cnt = 1;
 	put_data(&q2, data);
  }
}

/* print out some information for debug */
void info_print(void) {
  printf("\nVENUS: Input initial stimulators...\n");
  printf("Task1 actor - Addr = 0x%x\n", &task1_io);
  printf("Task2 actor - Addr = 0x%x\n", &task2_io);
  printf("Task3 actor - Addr = 0x%x\n", &task3_io);
  printf("Task4 actor - Addr = 0x%x\n", &task4_io);
  printf("Task5 actor - Addr = 0x%x\n", &task5_io);
  printf("Task6 actor - Addr = 0x%x\n", &task6_io);
  printf("\nSCHEDULER: Waiting for blocks to be ready...\n");
}


/*
 * Function: 
 *	1. Call task_exe()
 *	2. Put current block into idle fifo-queue
 * block_recycle(&block1)
 * block_n: address of block_n flags (a pointer)
 */
void block_recycle(block_f *n_block) {
  // 1. check if this block needs to recycle result
  if ((n_block->flags & BLOCK_INFLIGHT) != 0) {
    // 1.1 catch scheduler recycle handler's pointer
    Taskfunc task_callback = (Taskfunc)n_block->task_addr;
    // 1.2 call the handler with revelant actors
    task_callback(n_block->actor);
  }
  // 2. add current block into idle queue (if this block isn't in idle-fifo)
  if((n_block->flags & BLOCK_IDLE) == 0){
  	// 2.2 mark this block has been put into idle-fifo
  	_set_block_flag(n_block, BLOCK_IDLE);
  	put_queue(&q_block, (uint32_t)n_block);
  }
  
}

void actor_exe(void) {
  int current_task = 0;
  while(1){
  // scheduler would poll tasks only when VENUS has idle blocks
  if(queue_size(&q_block) >= 1){
  	printf("\nSCHEDULER: Current task is %d\n", current_task+1);
        block_f *n_block = (block_f *)get_queue(&q_block);
        _clear_block_flag(n_block);
  	task(task_io[current_task], n_block);
  	current_task = (current_task + 1) % NUM_TASKS;
  }
 }
}


void actor_launch(void) {
  actor_create();
  stimu_inject();
  info_print();
  actor_exe();
}

