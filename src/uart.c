#include "hw/uart.h"
#include "hw/addressmap.h"
#include "types.h"
#include "ulib.h"

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
  // 方法一. 读取FIFO(缓冲区)中的字节数，并与FIFO深度比较，如果填充水平小于FIFO深度，则说明FIFO有可用空间，可以继续发送数据。
  while (READ_BURST_32(SOC_UART0_ADDR, UART_TFL_OFFSET) < UART_FIFO_DEPTH)
    ;
  // 方法二. 读取LSR寄存器，并检查第五位(THRE)，该位可以指示FIFO缓冲区是否为空，如果THRE位为1，则说明FIFO有可用空间。因为LSR中含有缓冲区的其它信息，如果后续要用的话可以直接读这个。
  // uint32_t lsr = READ_BURST_32(SOC_UART0_ADDR, UART_LSR_OFFSET);
  // while (lsr & (1 << 5) == 0)
  //   ;
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

