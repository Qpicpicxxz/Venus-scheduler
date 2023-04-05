/* ref: https://github.com/plctlab/riscv-operating-system-mooc */
#include "common.h"

/* We just use these functions once, so declared here */
extern void heap_init(void);
extern void block_init(void);
extern void os_main(void);

void start_kernel(void) {
  heap_init();
  block_init();
  os_main();
}

