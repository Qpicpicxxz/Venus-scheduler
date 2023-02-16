#include "os.h"
#include "task.h"

extern void uart_init(void);
extern void page_init(void);
extern void mem_init(void);
//extern void schedule_main(void);

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, VENUS!\n");

  mem_init();
	_task1_start();
//	schedule_main();

	while (1) {}; // stop here!
}

