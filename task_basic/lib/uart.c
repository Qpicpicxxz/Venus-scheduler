#include "uart.h"
#include "device.h"
#include "types.h"
#include "ulib.h"

/* This is a uart library for task */

/* Function: Transmit a byte */
void uart_putc(char ch) {
  while (READ_BURST_32(BLOCK_DEBUGUART, UART_TFL_OFFSET) > UART_FIFO_DEPTH)
    ;
  WRITE_BURST_32(BLOCK_DEBUGUART, UART_THR_OFFSET, (uint32_t)ch);
}

/* Function: Transmit a string */
void uart_puts(char* s) {
  while (*s) {
    uart_putc(*s++);
  }
}



