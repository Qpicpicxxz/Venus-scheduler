#include "task.h"

// extern void trap_test(void);
// extern void page_test(void);

static actor_t task1_io;
static actor_t task2_io;
static actor_t task3_io;
static actor_t task4_io;
static actor_t task5_io;
static actor_t task6_io;
static fifo_t q1, q2, q3, q4, q5, q6, q7, q8;

/*
 * generate actors: taskx_io
 * Describe a task:
 * 	1. dependent token: a specific fifo {&qx}
 *	2. how many token needed in every dep_task's io: dep_num
 *	3. how many result would be produced: result_num
 *	4. where are the results going to go: a specific fifo {&qx}
 */
void actor_create(void) {
  /*
   * Describe example:
   * 	1. {&q1}: input fifo array
   *	2. 1: dependency number
   *	3. &q3: output fifo
   *	4. 32: result data length
   *	5. 1: task start address (so far, simulate)
   *	6. TASK1_END - TAKS1_START: task length
   */
  task1_io = (actor_t){{&q1},   
                       1,
                       &q3,
                       32,   
                       1, 	// Note: should be TASKn_START, NOT complete yet.I use it as a task token for simulation.
                       TASK1_END - TASK1_START};
  task2_io = (actor_t){{&q2}, 	// to represent dependency
                       1,
                       &q4,  
                       32,
                       2,
                       TASK2_END - TASK2_START};
  task3_io = (actor_t){{&q3, &q4}, 
                       2, 
                       &q5,  
                       32,
                       3,
                       TASK3_END - TASK3_START};
  task4_io = (actor_t){{&q3, &q4}, 
                       2, 
                       &q6,  
                       32,
                       4,
                       TASK3_END - TASK3_START};
  task5_io = (actor_t){{&q4}, 
                       1, 
                       &q7,  
                       32,
                       5,
                       TASK3_END - TASK3_START};
  task6_io = (actor_t){{&q5, &q6, &q7}, 
                       3, 
                       &q8,  
                       32,
                       6,
                       TASK3_END - TASK3_START};
}

/* inject original stimulators */
void stimu_inject(void) {
  void *p;
  uint32_t data_len = 32;
  for (int i = 7; i > 0; i--){
  	p = malloc(32);
  	*(int *)p = i;
 	put_data(&q1, (uint32_t)p, data_len);
  }
  for (int i = 7; i > 0; i--){
  	p = malloc(32);
  	*(int *)p = i;
 	put_data(&q2, (uint32_t)p, data_len);
  }
}

/* print out some information for debug */
void info_print(void) {
  printf("\nVENUS: Input initial stimulators...\n");
  // // every initial stimulates has its own address
  // printf("Data1 = %d --- Addr = 0x%x\n", *(int *)read_fifo(&q1),
  //        (uint32_t)(*read_addr_fifo(&q1)));
  // printf("Data2 = %d --- Addr = 0x%x\n", *(int *)read_else_fifo(&q1, 1),
  //        (uint32_t)(*read_elseaddr_fifo(&q1, 1)));
  // printf("Data3 = %d --- Addr = 0x%x\n", *(int *)read_fifo(&q2),
  //        (uint32_t)(*read_addr_fifo(&q2)));
  // printf("Data4 = %d --- Addr = 0x%x\n", *(int *)read_else_fifo(&q2, 1),
  //        (uint32_t)(*read_elseaddr_fifo(&q2, 1)));
  // printf("Data5 = %d --- Addr = 0x%x\n", *(int *)read_else_fifo(&q2, 2),
  //        (uint32_t)(*read_elseaddr_fifo(&q2, 2)));
  // every task actor(describe its dependencies) has its own address
  printf("Task1 actor - Addr = 0x%x\n", &task1_io);
  printf("Task2 actor - Addr = 0x%x\n", &task2_io);
  printf("Task3 actor - Addr = 0x%x\n", &task3_io);
  printf("Task4 actor - Addr = 0x%x\n", &task4_io);
  printf("Task5 actor - Addr = 0x%x\n", &task5_io);
  printf("Task6 actor - Addr = 0x%x\n", &task6_io);
  printf("\nSCHEDULER: Waiting for blocks to be ready...\n");
}

/* main RR denpendency checking loop */
void actor_exe(void) {
  while (1) {
    task(&task1_io);
    task(&task2_io);
    task(&task3_io);
    task(&task4_io);
    task(&task5_io);
    task(&task6_io);
  };
}

void actor_init(void) {
  actor_create();
  stimu_inject();
  info_print();
  actor_exe();
}

void user_task1(void) {
  uart_puts("Task 1: Created!\n");
  while (1) {
    uart_puts("Task 1: Running...\n");
    task_delay(DELAY);
    task_yield();
  }
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void) {
  task_create(actor_init);
  // task_create(user_task1);
}

