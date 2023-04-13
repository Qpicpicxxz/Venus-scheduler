/* ref: https://github.com/mit-pdos/xv6-riscv/blob/riscv/user/ulib.c */
#include "ulib.h"

void delay(volatile int count) {
  count *= DELAY;
  while (count--)
    ;
}


