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

/* Function: Receive a byte */
uint8_t uart_recvc(uint32_t uart_addr) {
  return (uint8_t)READ_BURST_32(uart_addr, UART_RBR_OFFSET);
}

/* Function: Receive a byte on software */
uint8_t uart_revc_sw(uint32_t uart_addr) {
  uint32_t receive_fifo_level = READ_BURST_32(uart_addr, UART_RFL_OFFSET);
  while (receive_fifo_level == 0) {
    // delay(100000);
    printf("VENUS_SOC_CPU: Uart_receive_byte timeout! uart_addr = %d", uart_addr);
    return -1;
  }
  return (uint8_t)READ_BURST_32(uart_addr, UART_RBR_OFFSET);
}

void uart_irq_handler(void) {
  uint8_t iid = (uint8_t)READ_BURST_32(BLOCK_DEBUGUART, UART_IIR_OFFSET);
  switch (iid) {
  case 0b0000:
    printf("VENUS_BLOCK: modem status interrupt detected! $stop\n");
  case 0b0010:
    printf("VENUS_BLOCK: THR empty interrupt detected! $stop\n");
  case 0b0100: {
    printf("VENUS_BLOCK: received data available interrupt detected! $stop\n");
    uint8_t receive_data_byte = uart_recvc(BLOCK_DEBUGUART);
    printf("VENUS_BLOCK: recv char is: %s $stop\n", receive_data_byte);
  }
  case 0b0110:
    printf("VENUS_BLOCK: receiver line status interrupt detected! $stop\n");
  case 0b0111:
    printf("VENUS_BLOCK: busy detect interrupt detected! $stop\n");
  case 0b1100:
    printf("VENUS_BLOCK: character timeout interrupt detected! $stop\n");
  default:
    printf("VENUS_BLOCK: unknown uart interrupt type! $stop\n");
  }
}

