#include "assert.h"
#include "types.h"

extern int printf(const char* s, ...);
// extern void uart_init(void);
extern void uart_putc(char ch);
extern void uart_puts(char* s);

void start_kernel(void) {
  // uart_init();
  printf("Line: %d, File: %s, Date: %d, Time: %d"); 
  uart_putc(35);
  uart_putc(0);  // would print out "#"
  uart_puts("string\n");
  printf("here is printf!\n");
  uart_puts("$stop\n");
  int a = 1;
  assert(a + 1 == 3);
  asm volatile("ebreak");
}

