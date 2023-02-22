#include "os.h"

// The following functions would be called ONLY ONCES here, so I jusst declared here ONCE but NOT included in file "os.h".
extern void uart_init(void);
extern void page_init(void);
extern void sched_init(void);
extern void mem_init(void);
extern void schedule(void);
extern void os_main(void);

void start_kernel(void)
{
	uart_init();
	uart_puts("Hello, VENUS!\n");
	// print out memory distribution
	mem_init();
	// initialize page allocation and print out heap range
	page_init();
	sched_init();
	os_main();
	schedule();
	// stop here!
	while (1){};
}

