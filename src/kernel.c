#include "assert.h"
#include "types.h"

extern void devctrl_init(void);
extern void venus_uart_init(void);
extern void irq_init(void);

extern int printf(const char* s, ...);
extern void uart_init(void);
extern void uart_putc(char ch);
extern void uart_puts(char* s);
void test_uart(void);
void test_irq(void);

void start_kernel(void) {
  // 1. 解开 Venus DMA 以及所有的 Venus Block 的硬复位
  devctrl_init();
  // 2. Scheduler 初始化配置 Venus(self + blocks) 的 UART (actual sence only)
  // venus_uart_init();
  // 3. Scheduler 初始化interrupt (unmask开启需要Venus使用的中断位)
  irq_init();
  
  
  
  
  // test_uart();
  test_irq();
}

void test_irq(void) {
  asm volatile("ebreak");
}

void test_uart(void) {
  /* UART + printf + $stop + assert test */
  printf("Test printf $stop\n");
  uart_puts("Test uart transmit string $stop\n");
  uart_putc(0x24);
  uart_putc(0x73);
  uart_putc(0x74);
  uart_putc(0x6f);
  uart_putc(0x70);
  uart_putc(0x0);
  /* [Software printf] VENUS_SCHEDULER: Assertion failed: a + 1 == 3 */
  int a = 1;
  assert(a + 1 == 3);
  asm volatile("ebreak");
}
