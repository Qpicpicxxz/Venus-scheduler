#include "os.h"
#include "fifo.h"

#define DELAY 1000

void user_task0(void)
{
	uart_puts("Task 0: Created!\n");
	// use static variables to avoid stack overflow
	static fifo_t F1;
	init_fifo(&F1); // zero wptr, rptr
	put_fifo(&F1, 5);
	put_fifo(&F1, 6);
	printf("fifo size: %d\nand get token: %d\nthen fifo size:%d\n", fifo_size(&F1), get_fifo(&F1), fifo_size(&F1));
	while (1)
	{
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
	task_create(user_task0);
	task_create(user_task1);
}

