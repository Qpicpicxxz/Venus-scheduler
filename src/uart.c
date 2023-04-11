#include "hw/addressmap.h"
#include "hw/uart.h"
#include "ulib.h"
#include "types.h"

void uart_init(void) {
  // Write to Modem Control Register (MCR) to program SIR mode, auto flow, loopback, modem control outputs
  WRITE_BURST_32(SOC_UART0_ADDR, UART_MCR_OFFSET, 0x0);
  // LCR enable
  WRITE_BURST_32(SOC_UART0_ADDR, UART_LCR_OFFSET, 0x80);
  // DLL set required boud rate (11520 Baud)
  WRITE_BURST_32(SOC_UART0_ADDR, UART_DLL_OFFSET, 0x1b);
  // DLH set required baud rate
  WRITE_BURST_32(SOC_UART0_ADDR, UART_DLH_OFFSET, 0x00);
  // LCR
  WRITE_BURST_32(SOC_UART0_ADDR, UART_LCR_OFFSET, 0x03);
  // FCR enable FIFOs and set Receive FIFO threshold level
  WRITE_BURST_32(SOC_UART0_ADDR, UART_FCR_OFFSET, 0x01);
  // Write to IER to enable required interrupts
  WRITE_BURST_32(SOC_UART0_ADDR, UART_IER_OFFSET, 0x81);
}

void uart_putc(char ch) {
  /*while (1) {
    if (READ_BURST_32(SOC_UART0_ADDR, UART_TFL_OFFSET) < UART_FIFO_DEPTH)
      break;
  }*/
  WRITE_BURST_32(SOC_UART0_ADDR, UART_THR_OFFSET, (uint32_t)ch);
}

void uart_puts(char* s) {
  while (*s) {
    uart_putc(*s++);
  }
}

uint8_t uart_recvc(void) {
  return (uint8_t)READ_BURST_32(SOC_UART0_ADDR, UART_RBR_OFFSET);
}
