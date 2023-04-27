/* ref: https://github.com/mit-pdos/xv6-riscv/blob/riscv/user/ulib.c */
#include "ulib.h"

void delay(volatile int count) {
  count *= DELAY;
  while (count--)
    ;
}

/* Function: Memory initialization */
void* memset(void* dst, int c, uint32_t n) {
  char* cdst = (char*)dst;
  int i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  return dst;
}
