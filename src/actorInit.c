#include "task.h"
/* 
 * Note: Later optimization should change 
 *	these actor's initializaion into dynamically allocation.
 */
static actor_t task1_io;
static actor_t task2_io;
static actor_t task3_io;
static actor_t task4_io;
static actor_t task5_io;
static actor_t task6_io;
static list_t *actor_l;
static fifo_t q1, q2, q3, q4, q5, q6, q7, q8, q9, q10, q11;

queue_t block_q;
uint8_t actor_index;

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
   *	2. 1: dependency number	[user-specified]
   *	3. {&q3}: output fifo
   *	4. 4: result data length (byte) [user-specified]
   *	5. TASK1_START: task address
   *	6. TASK1_END - TAKS1_START: task length
   */
  task1_io = (actor_t){{&q1},   
                       1,
                       {&q3, &q4},
                       2,
                       4,   
                       TASK1_START,
                       TASK1_END - TASK1_START};
  task2_io = (actor_t){{&q2},
                       1,
                       {&q5, &q6, &q7},
                       3,
                       4,
                       TASK2_START,
                       TASK2_END - TASK2_START};
  task3_io = (actor_t){{&q3, &q5}, 
                       2, 
                       {&q8},
                       1,  
                       4,
                       TASK3_START,
                       TASK3_END - TASK3_START};
  task4_io = (actor_t){{&q4, &q6}, 
                       2, 
                       {&q9},
                       1, 
                       4,
                       4,
                       TASK3_END - TASK3_START};
  task5_io = (actor_t){{&q7}, 
                       1, 
                       {&q10},  
                       1,
                       4,
                       TASK1_START,
                       TASK1_END - TASK1_START};
  task6_io = (actor_t){{&q8, &q9, &q10}, 
                       3, 
                       {&q11},
                       1, 
                       4,
                       TASK2_START,
                       TASK2_END - TASK2_START};
}

/* create a linkedlist actor_l and link all the actors */
void actorlist_create(void) {
	actor_l = create_list();
	// first linked actor was placed at the end of the list
	insert(actor_l, create_node((uint32_t)&task1_io));
	insert(actor_l, create_node((uint32_t)&task2_io));
	insert(actor_l, create_node((uint32_t)&task3_io));
	insert(actor_l, create_node((uint32_t)&task4_io));
	insert(actor_l, create_node((uint32_t)&task5_io));
	insert(actor_l, create_node((uint32_t)&task6_io));
}

/* inject original stimulators */
void stimu_inject(void) {
  void *p;
  uint32_t data_len = 4;	// 4byte is the length of 'int'
  // every inject can act as a packet's arrival
  for (int i = 3; i > 0; i--){
  	// 1.1 alloc data memory
  	p = malloc(data_len);
  	// 1.2 initialize data
  	*(int *)p = i + 10;
  	// 1.3 catch input data's address
  	uint32_t alloc_addr = (uint32_t)p;
  	// 2.1 define and allocate data descriptor
  	data_t* data = malloc(sizeof *data);
  	// 2.2 initialize data descriptor
  	data->ptr = alloc_addr;
  	data->len = data_len;
  	data->cnt = 1;
  	// 3. put data descriptor's pointer into stimulate fifo
 	put_data(&q1, data);
  }
  /*
  for (int i = 1; i > 0; i--){
  	p = malloc(data_len);
  	*(int *)p = 9;
  	uint32_t alloc_addr = (uint32_t)p;
  	data_t* data = malloc(sizeof *data);
  	data->ptr = alloc_addr;
  	data->len = data_len;
  	data->cnt = 1;
 	put_data(&q2, data);
  }*/
}

/* print out some information for debug */
void info_print(void) {
  printf(GREEN("\nVENUS: Input initial stimulators...\n"));
  printf("Task1 actor - Addr = 0x%x\n", &task1_io);
  printf("Task2 actor - Addr = 0x%x\n", &task2_io);
  printf("Task3 actor - Addr = 0x%x\n", &task3_io);
  printf("Task4 actor - Addr = 0x%x\n", &task4_io);
  printf("Task5 actor - Addr = 0x%x\n", &task5_io);
  printf("Task6 actor - Addr = 0x%x\n", &task6_io);
  printf("\nSCHEDULER: Waiting for blocks to be ready...\n");
}

/* Funcition: Poll tasks only when VENUS has idle blocks */
void actor_exe(void) {
  // 1. initialize p to the last (first in) node
  link p = actor_l->tail->prev;
  while(1){
  // 2. wait for idle blocks
  if (queue_size(&block_q) >= 1) {
    uint8_t fire = 0;
    // 3. idle blocks found, push out and use it
    block_f *n_block = (block_f *)get_queue(&block_q);
    // 5. polling for ready task
    while (!fire) {
    	// 6. make list into a ring queue
    	if (p == actor_l->head)
    		p = actor_l->tail->prev;
    	// 7. compute actor's index
    	// Note: Careful of this cauculation...
        actor_index =
            ((uint32_t)p->item - (uint32_t)(actor_l->tail->prev)->item) /
            sizeof(actor_t) + 1;
        printf(" %d", actor_index);
        task_delay(10000);
        // 8. task dependency checking and catch if fire success or not
        fire = task((actor_t *)p->item, n_block);
        // 9. check next actor
        p = p->prev;
      }
    }
  }
}

void actor_launch(void) {
  actor_create();
  actorlist_create();
  stimu_inject();
  info_print();
  actor_exe();
}

