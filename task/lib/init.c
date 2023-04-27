#include "common.h"

extern void heap_init(void);
extern void main(void);

void block_start(void) {
  heap_init();
  main();
}
