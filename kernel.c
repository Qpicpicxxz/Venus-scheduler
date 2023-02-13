#include "os.h"

/*
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 */
extern void uart_init(void);
extern void page_init(void);
extern void mem_init(void);
extern void os_main(void);

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, VENUS!\n");

	mem_init();
	os_main();

	while (1) {}; // stop here!
}

