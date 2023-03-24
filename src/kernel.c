/* ref: https://github.com/plctlab/riscv-operating-system-mooc */
#include "os.h"

/* We just use these functions once, so declared here */
extern void uart_init(void);
extern void mem_init(void);	// print out memory distribution
extern void heap_init(void);
extern void trap_init(void);	// put trap handler entry address to mtvec
// extern void page_init(void);	// initialize page allocation and print out heap range
extern void sched_init(void);
extern void init_block_spm(void);
extern void os_main(void);
extern void schedule(void);
extern void malloc_test(void);
extern void link_test(void);
extern void memcpy_test(void);

void start_kernel(void) {
  uart_init();
  uart_puts(GREEN("Hello, VENUS!\n"));
  // assert(1==2);
  mem_init();
  heap_init();
   //malloc_test();
   // link_test();
  // memcpy_test();
    //while(1){};
  trap_init();
  // page_init();
  sched_init();
  init_block_spm();
  os_main();
  schedule();
  while (1) {};
}

