#include "assert.h"
#include "types.h"

extern int printf(const char* s, ...);
#ifndef SIM_VCS
extern void uart_init(void);
#endif
extern void uart_putc(char ch);
extern void uart_puts(char* s);

void start_kernel(void) {
#ifndef SIM_VCS
  uart_init();
#endif
  // printf("Line: %d, File: %s, Date: %d, Time: %d");
  uart_puts("$stop\0\n");
  printf("missed first stop\n");
  uart_putc(0x24);
  uart_putc(0x73);
  uart_putc(0x74);
  uart_putc(0x6f);
  uart_putc(0x70);
  uart_putc(0x0);  // can not pause simulation 不懂啊，为什么啊
  uart_putc(35);
  uart_putc(0);  // would print out "#"
  uart_puts("string\n");
  printf("here is printf!\n");
  /* [Software printf] VENUS_SCHEDULER: Assertion failed: a + 1 == 3 */
  int a = 1;
  assert(a + 1 == 3);
  asm volatile("ebreak");
}

