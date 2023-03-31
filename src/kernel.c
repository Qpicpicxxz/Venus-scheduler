/* ref: https://github.com/plctlab/riscv-operating-system-mooc */
#include "common.h"

/* We just use these functions once, so declared here */
extern void uart_init(void);
extern void mem_info(void);  // print out memory distribution
extern void heap_init(void);
extern void trap_init(void);
extern void sched_init(void);
extern void spm_init(void);
extern void os_main(void);

/* test function */
extern void malloc_test(void);
extern void link_test(void);
extern void memcpy_test(void);
extern void malloc_64_test(void);
extern void dma_test(void);

void start_kernel(void) {
  uart_init();
  printf(GREEN("Hello, VENUS!\n"));
  mem_info();
  heap_init();
  trap_init();
  sched_init();
  spm_init();
  // malloc_test();
  // link_test();
  // memcpy_test();
  // malloc_64_test();
   dma_test();
  // while(1){};
  os_main();
}

