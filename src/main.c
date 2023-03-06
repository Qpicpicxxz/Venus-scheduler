#include "task.h"

// extern void trap_test(void);
// extern void page_test(void);

static actor_t task1_io;
static actor_t task2_io;
static actor_t task3_io;
static fifo_t q1, q2, q3;

/*
 * generate actors: taskx_io
 * Describe a task:
 * 	1. dependent task's io: taskn_io.in[n] (dep_task's token)
 *	2. how many token needed in every dep_task's io
 *	3. the length of each token
 *	4. output task's io: taskn_io.out[n] -> taskn_io.out[n]
 */
void actor_create(void) {
  /*
   * Describe example:
   * 	1. q1: task1_io.in[0]
   *	2. q2: task1_io.out[0]
   *	3. dep_num = 1
   *	4. result_num = 1
   */
  task1_io = (actor_t){{&q1}, 
                       {&q2},         
                       {1},
                       {1},   
                       TASK1_START, 
                       TASK1_END - TASK1_START};
  task2_io = (actor_t){{&q2}, 
                       {&q3}, 
                       {2}, 
                       {1},
                       TASK2_START,
                       TASK2_END - TASK2_START};
  task3_io = (actor_t){{&q3}, 
                       {&q1}, 
                       {2}, 
                       {4},
                       TASK3_START,
                       TASK3_END - TASK3_START};
}

/* inject original stimulators */
void stimu_inject(void) {
  // 1. allocate memory space
  void *p = malloc(32);
  // 2. assignment
  *(int *)p = 4;
  // 3. put into first task's fifo as stimulator
  //q1.data[0]->ptr = 1;
  uint32_t data_len = 32;
  put_data(&q1, (uint32_t)p, data_len);
  p = malloc(data_len);
  *(int *)p = 3;
  put_data(&q1, (uint32_t)p, data_len);
  p = malloc(data_len);
  *(int *)p = 2;
  put_data(&q1, (uint32_t)p, data_len);
  p = malloc(data_len);
  *(int *)p = 1;
  put_data(&q1, (uint32_t)p, data_len);
}

/* print out some information for debug */
void info_print(void) {
  printf("\nVENUS: Input initial stimulators...\n");
  // every initial stimulates has its own address
  printf("Data1 = %d --- Addr = 0x%x\n", *(int *)read_fifo(&q1),
         (uint32_t)(*read_addr_fifo(&q1)));
  printf("Data2 = %d --- Addr = 0x%x\n", *(int *)read_else_fifo(&q1, 1),
         (uint32_t)(*read_elseaddr_fifo(&q1, 1)));
  printf("Data3 = %d --- Addr = 0x%x\n", *(int *)read_else_fifo(&q1, 2),
         (uint32_t)(*read_elseaddr_fifo(&q1, 2)));
  printf("Data4 = %d --- Addr = 0x%x\n", *(int *)read_else_fifo(&q1, 3),
         (uint32_t)(*read_elseaddr_fifo(&q1, 3)));
  // every task actor(describe its dependencies) has its own address
  printf("Task1 actor - Addr = 0x%x\n", &task1_io);
  printf("Task2 actor - Addr = 0x%x\n", &task2_io);
  printf("Task3 actor - Addr = 0x%x\n", &task3_io);
  printf("\nSCHEDULER: Waiting for blocks to be ready...\n");
}

/* main RR denpendency checking loop */
void actor_exe(void) {
  while (1) {
    task1(&task1_io);
    task2(&task2_io);
    task3(&task3_io);
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

