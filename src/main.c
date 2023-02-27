#include "actor.h"

uint32_t data1 = 4;
uint32_t data2 = 3;
uint32_t data3 = 2;
uint32_t data4 = 1;
extern void trap_test(void);
extern void page_test(void);
static actorio_t task1_io;
static actorio_t task2_io;
static actorio_t task3_io;
static fifo_t q1, q2, q3, q4, q5, q6;
void actor_exe(void) {
  while (1) {
    task1(&task1_io, &task2_io);
    task2(&task2_io, &task3_io);
    task3(&task3_io, &task1_io);
  };
}
void actor_init(void) {
  // page_test();
  uart_puts("\nVENUS: Started...\n");
  // trap_test();
  //  use static variables to avoid stack overflow
  //  static keyword means that it can only be called from other functions in
  //  the same source file
  // generate an actor task_io
  // q1: task1_io.in[0]  q5: task1_io.in[1] q2: task1_io.out[0] else: NULL
  task1_io = (actorio_t){{&q1}, {&q2}};
  task2_io = (actorio_t){{&q3}, {&q4}};
  task3_io = (actorio_t){{&q5}, {&q6}};
  put_fifo(&q1, (uint32_t)&data1);
  put_fifo(&q1, (uint32_t)&data2);
  put_fifo(&q1, (uint32_t)&data3);
  put_fifo(&q1, (uint32_t)&data4);
  printf("\nVENUS: Input initial stimulators...\n");
  printf("data1 = %d addr = 0x%x\n", data1, &data1);
  printf("data2 = %d addr = 0x%x\n", data2, &data2);
  printf("data3 = %d addr = 0x%x\n", data3, &data3);
  printf("data4 = %d addr = 0x%x\n", data4, &data4);
  printf("task1 actor addr = 0x%x\n", &task1_io);
  // printf("in main %d\n", fifo_size(task1_io.in[0]));
  printf("task2 actor addr = 0x%x\n", &task2_io);
  printf("task3 actor addr = 0x%x\n", &task3_io);
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

