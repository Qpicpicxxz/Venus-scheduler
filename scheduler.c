#include "os.h"
#include "fifo.h"

uint32_t data1 = 888;
uint32_t data2 = 555;
uint32_t data3;
uint32_t data4;

void actor_task(void)
{
  uart_puts("VENUES: Started\n");
  // use static variables to avoid stack overflow
  static fifo_t q1, q2, q3, q4, q5, q6;
  // create an actor task_io
  // q1: task1_io.in[0]  q5: task1_io.in[1] q2: task1_io.out[0] else: NULL
  static actorio_t task1_io = {{&q1, &q5}, {&q2}};
  static actorio_t task2_io = {{&q3}, {&q4, &q6}};
  // zero wptr, rptr
  init_fifo(&q1);
  init_fifo(&q2);
  put_fifo(&q1, (uint32_t)&data1);
  put_fifo(&q5, (uint32_t)&data2);
  printf("data1 = %d\n", data1);
  printf("data2 = %d\n", data2);
  while (1)
  {
    task1(&task1_io, &task2_io);
    task2(&task2_io);
  };
}

void user_task1(void)
{
  uart_puts("Task 1: Created!\n");
  while (1)
  {
    uart_puts("Task 1: Running...\n");
    task_delay(DELAY);
    task_yield();
  }
}

/* NOTICE: DON'T LOOP INFINITELY IN main() */
void os_main(void)
{
  task_create(actor_task);
  task_create(user_task1);
}

