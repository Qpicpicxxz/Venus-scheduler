#include "os.h"

/* We just use these functions once, so declared here */
extern void uart_init(void);
extern void mem_init(void);	// print out memory distribution
extern void trap_init(void);	// put trap handler entry address to mtvec
extern void page_init(void);	// initialize page allocation and print out heap range
extern void sched_init(void);
extern void init_block_spm(void);
extern void os_main(void);
extern void schedule(void);

void start_kernel(void) {
  uart_init();
  uart_puts("Hello, VENUS!\n");
  mem_init();
  trap_init();
  page_init();
  sched_init();
  init_block_spm();
  os_main();
  schedule();
  while (1) {};
}

