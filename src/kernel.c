#include "os.h"

/*
   The following functions would be called ONLY ONCES here, so I just
   declared here ONCE but NOT included in file "os.h".
*/
extern void uart_init(void);
extern void page_init(void);
extern void sched_init(void);
extern void mem_init(void);
extern void trap_init(void);
extern void schedule(void);
extern void init_block_spm(void);
extern void os_main(void);

void start_kernel(void) {
  uart_init();
  uart_puts("Hello, VENUS!\n");
  // print out memory distribution
  mem_init();
  // put trap handler entry address to mtvec
  trap_init();
  // initialize page allocation and print out heap range
  page_init();
  sched_init();
  init_block_spm();
  os_main();
  schedule();
  // stop here!
  while (1) {};
}

