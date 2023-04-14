/* Corresponding to HW-BFM src/testbench/peripherals/uart_task_bfm.svh */
#include "hw/uart.h"
#include "hw/addressmap.h"
#include "types.h"
#include "ulib.h"

// VENUS_DEBUG_UART_ADDR is VCS console log

/* Function: Transmit a byte */
void uart_putc(char ch) {
  // Option 1. 读取FIFO(缓冲区)中的字节数，并与FIFO深度比较，如果填充水平小于FIFO深度，则说明FIFO有可用空间，可以继续发送数据。
  while (READ_BURST_32(VENUS_DEBUG_UART_ADDR, UART_TFL_OFFSET) < UART_FIFO_DEPTH)
    ;
  // Option 2. 读取LSR寄存器，并检查第五位(THRE)，该位可以指示FIFO缓冲区是否为空，如果THRE位为1，则说明FIFO有可用空间。因为LSR中含有缓冲区的其它信息，如果后续要用的话可以直接读这个。
  // uint32_t lsr = READ_BURST_32(VENUS_DEBUG_UART_ADDR, UART_LSR_OFFSET);
  // while (lsr & (1 << 5) == 0)
  //   ;
  WRITE_BURST_32(VENUS_DEBUG_UART_ADDR, UART_THR_OFFSET, (uint32_t)ch);
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
    delay(100000);
    printf("VENUS_SOC_CPU: Uart_receive_byte timeout! uart_addr = %d", uart_addr);
    return -1;
  }
  return (uint8_t)READ_BURST_32(uart_addr, UART_RBR_OFFSET);
}

void uart_irq_handler(uint32_t uart_addr) {
  uint8_t iid = (uint8_t)READ_BURST_32(uart_addr, UART_IIR_OFFSET);
  switch (iid) {
  case 0b0000:
    printf("VENUS_SOC_CPU: modem status interrupt detected! $stop\n");
  case 0b0010:
    printf("VENUS_SOC_CPU: THR empty interrupt detected! $stop\n");
  case 0b0100: {
    printf("VENUS_SOC_CPU: received data available interrupt detected! $stop\n");
    uint8_t receive_data_byte = uart_recvc(uart_addr);
    printf("[VENUS_SOC_UART0: recv char is: %s $stop\n", receive_data_byte);
  }
  case 0b0110:
    printf("VENUS_SOC_CPU: receiver line status interrupt detected! $stop\n");
  case 0b0111:
    printf("VENUS_SOC_CPU: busy detect interrupt detected! $stop\n");
  case 0b1100:
    printf("VENUS_SOC_CPU: character timeout interrupt detected! $stop\n");
  default:
    printf("VENUS_SOC_CPU: unknown uart interrupt type! $stop\n");
  }
}

void uart_init(uint32_t uart_addr) {
  // Write to Modem Control Register (MCR) to program SIR mode, auto flow, loopback, modem control outputs
  WRITE_BURST_32(uart_addr, UART_MCR_OFFSET, 0x0);
  // LCR enable
  WRITE_BURST_32(uart_addr, UART_LCR_OFFSET, 0x80);
  // DLL set required boud rate (11520 Baud)
  WRITE_BURST_32(uart_addr, UART_DLL_OFFSET, 0x1b);
  // DLH set required baud rate
  WRITE_BURST_32(uart_addr, UART_DLH_OFFSET, 0x00);
  // LCR
  WRITE_BURST_32(uart_addr, UART_LCR_OFFSET, 0x03);
  // FCR enable FIFOs and set Receive FIFO threshold level
  WRITE_BURST_32(uart_addr, UART_FCR_OFFSET, 0x01);
  // Write to IER to enable required interrupts
  WRITE_BURST_32(uart_addr, UART_IER_OFFSET, 0x81);
}

void venus_uart_init(void) {
  // Scheduler config itself debug uart
  uart_init(SOC_UART0_ADDR);
  // Scheduler config all the block's debug uart
  for (int i = 0; i < NUM_BLOCKS; i++) {
    for (int j = 0; j < NUM_CLUSTERS; j++) {
      uart_init(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(i) + BLOCK_OFFSET(j) + BLOCK_DEBUGUART_OFFSET);
    }
  }
}

