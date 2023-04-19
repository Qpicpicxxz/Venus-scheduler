#ifndef TEST_VENUS_UART
#include "common.h"
#include "platform.h"
/*
 * The UART control registers are memory-mapped at address UART0.
 * This macro returns the address of one of the registers.
 */
#define UART_REG(reg) ((volatile uint8_t*)(UART0 + reg))

/* ref: TECHNICAL DATA ON 16550, http://byterunner.com/16550.html */

/*
 * UART control registers map. see [1] "PROGRAMMING TABLE"
 * note some are reused by multiple functions
 * 0 (write mode): THR/DLL
 * 1 (write mode): IER/DLM
 */
#define RHR 0  // Receive Holding Register (read mode)
#define THR 0  // Transmit Holding Register (write mode)
#define DLL 0  // LSB of Divisor Latch (write mode)
#define IER 1  // Interrupt Enable Register (write mode)
#define DLM 1  // MSB of Divisor Latch (write mode)
#define FCR 2  // FIFO Control Register (write mode)
#define ISR 2  // Interrupt Status Register (read mode)
#define LCR 3  // Line Control Register
#define MCR 4  // Modem Control Register
#define LSR 5  // Line Status Register
#define MSR 6  // Modem Status Register
#define SPR 7  // ScratchPad Register

/*
 * POWER UP DEFAULTS
 * IER = 0: TX/RX holding register interrupts are both disabled
 * ISR = 1: no interrupt penting
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0, BITS 4-7 = inputs
 * FCR = 0
 * TX = High
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTR = High
 * RXRDY = High
 * TXRDY = Low
 * INT = Low
 */

/*
 * LINE STATUS REGISTER (LSR)
 * LSR BIT 0:
 * 0 = no data in receive holding register or FIFO.
 * 1 = data has been receive and saved in the receive holding register or FIFO.
 * ......
 * LSR BIT 5:
 * 0 = transmit holding register is full. 16550 will not accept any data for transmission.
 * 1 = transmitter hold register (or FIFO) is empty. CPU can load the next character.
 * ......
 */
#define LSR_RX_READY (1 << 0)
#define LSR_TX_IDLE (1 << 5)

#define uart_read_reg(reg) (*(UART_REG(reg)))
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v))

void uart_init() {
  // disable interrupts
  uart_write_reg(IER, 0x00);
  uint8_t lcr = uart_read_reg(LCR);
  uart_write_reg(LCR, lcr | (1 << 7));
  uart_write_reg(DLL, 0x03);
  uart_write_reg(DLM, 0x00);

  lcr = 0;
  uart_write_reg(LCR, lcr | (3 << 0));

  // enalbe receive interrupts
  uint8_t ier = uart_read_reg(IER);
  uart_write_reg(IER, ier | (1 << 0));
}

int uart_putc(char ch) {
  while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0)
    ;
  return uart_write_reg(THR, ch);
}

void uart_puts(char* s) {
  while (*s) {
    uart_putc(*s++);
  }
}

// we have to remove this inturrpt, otherwise it will triggle forever
int uart_getc(void) {
  if (uart_read_reg(LSR) & LSR_RX_READY) {
    return uart_read_reg(RHR);
  } else {
    return -1;
  }
}
#else
#include "common.h"
#include "hw/addressmap.h"
#include "hw/uart.h"
#include "ulib.h"

void uart_init() {
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
  while (1) {
    if (READ_BURST_32(SOC_UART0_ADDR, UART_TFL_OFFSET) < UART_FIFO_DEPTH)
      break;
  }
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

// void uart_interrupt_handler() {
//   uint32_t IIR = READ_BURST_32(SOC_UART0_ADDR, UART_IIR_OFFSET);
//   uint8_t IID  = 0xf0 & (uint8_t)IIR;
//   switch (IID) {
//   case 0b0000:
//     printf("[Hardware] VENUS_SOC_CPU: modem status interrupt detected!\n");
//   case 0b0010:
//     printf("[Hardware] VENUS_SOC_CPU: THR empty interrupt detected!\n");
//   case 0b0100: {
//     printf("[Hardware] VENUS_SOC_CPU: received data available interrupt detected!\n");
//     printf("[Hardware] VENUS_SOC_UART0: recv char is: %s", uart_recvc());
//   }
//   case 0b0110:
//     printf("[Hardware] VENUS_SOC_CPU: receiver line status interrupt detected!\n");
//   case 0b0111:
//     printf("[Hardware] VENUS_SOC_CPU: busy detect interrupt detected!\n");
//   case 0b1100:
//     printf("[Hardware] VENUS_SOC_CPU: character timeout interrupt detected!\n");
//   }
// }
#endif  // TEST_VENUS_UART

