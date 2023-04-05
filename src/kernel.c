/* ref: https://github.com/plctlab/riscv-operating-system-mooc */
#include "common.h"

/* We just use these functions once, so declared here */
extern void uart_init(void);
extern void mem_info(void);  // print out memory distribution
extern void heap_init(void);
extern void trap_init(void);
extern void block_init(void);
extern void os_main(void);

/* test function */
extern void malloc_test(void);
extern void link_test(void);
extern void memcpy_test(void);
extern void malloc_64_test(void);

void start_kernel(void) {
  uart_init();
  printf(GREEN("Hello, VENUS!\n"));
  mem_info();
  heap_init();
  trap_init();
  block_init();
  // malloc_64_test();
  // malloc_test();
  // link_test();
  // memcpy_test();
  // while(1){};
  os_main();
}

