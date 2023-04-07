#include "common.h"

/* Function: remove keyboard interrupt in uart.c */
extern int uart_getc(void);

/*
 * Function:
 * 	1. recycle idle blocks
 * 	2. pass the computing result to the successor
 */
extern void block_handler(uint32_t block_index);
extern void dma_transfer_done_handler(uint32_t channen_index);

/* handle external interrupt */
void external_interrupt_handler() {
  /*
   * I use kayboard interrupt to simulate block interrupt,
   * so when I press a key, it would rise a interrupt,
   * and uart_getc() function acts as to clear this interrupt
   */
  while (1) {
    int c = uart_getc();
    /*
     * I use different key to represent different block's signal
     * suppose there are 8 VENUS blocks
     * ------------------------------------------
     *   1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |
     * ------------------------------------------
     *  49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 |
     * ------------------------------------------
     */
    if (c == -1) {
      break;
    } else {
      /*
       * TODO: to detect the repetition interrupt from same core
       * to prevent same core be put in the idle-queue multiple times
       */
      switch ((int)c) {
      case 49:
        printf("\nBLOCK 1: Interrupt...\n");
        block_handler(0);
      default:
        printf("UNKNOWN external interruption!\n");
        break;
      }
    }
  }
}

