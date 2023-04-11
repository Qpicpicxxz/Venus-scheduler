#include "block.h"
#include "common.h"
#include "saddr.h"

/* Function: remove keyboard interrupt in uart.c */
extern int uart_getc(void);

/*
 * Function:
 * 	1. recycle idle blocks
 * 	2. pass the computing result to the successor
 */
extern void block_handler(block_t* block);
extern void dma_transfer_done_handler(uint32_t channen_index);

uint32_t block_start;

/* define all the block's flag --- allocate their memory space */
static block_t block1;
static block_t block2;
static block_t block3;
static block_t block4;
static block_t block5;
static block_t block6;
static block_t block7;
static block_t block8;

void spm_init() {
  block1.spm_addr = SPM1_ADDR;
  block2.spm_addr = SPM2_ADDR;
  block3.spm_addr = SPM3_ADDR;
  block4.spm_addr = SPM4_ADDR;
  block5.spm_addr = SPM5_ADDR;
  block6.spm_addr = SPM6_ADDR;
  block7.spm_addr = SPM7_ADDR;
  block8.spm_addr = SPM8_ADDR;
  block_start     = (uint32_t)&block1;
}

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
        block_handler(&block1);
        break;
      case 50:
        printf("\nBLOCK 2: Interrupt...\n");
        block_handler(&block2);
        break;
      case 51:
        printf("\nBLOCK 3: Interrupt...\n");
        block_handler(&block3);
        break;
      case 52:
        printf("\nBLOCK 4: Interrupt...\n");
        block_handler(&block4);
        break;
      case 53:
        printf("\nBLOCK 5: Interrupt...\n");
        block_handler(&block5);
        break;
      case 54:
        printf("\nBLOCK 6: Interrupt...\n");
        block_handler(&block6);
        break;
      case 55:
        printf("\nBLOCK 7: Interrupt...\n");
        block_handler(&block7);
        break;
      case 56:
        printf("\nBLOCK 8: Interrupt...\n");
        block_handler(&block8);
        break;
      case 113:
        printf("\nDMA CH0: Interrupt...\n");
        dma_transfer_done_handler(0);
        break;
      case 119:
        printf("\nDMA CH1: Interrupt...\n");
        dma_transfer_done_handler(1);
        break;
      case 101:
        printf("\nDMA CH2: Interrupt...\n");
        dma_transfer_done_handler(2);
        break;
      case 114:
        printf("\nDMA CH3: Interrupt...\n");
        dma_transfer_done_handler(3);
        break;
      case 116:
        printf("\nDMA CH4: Interrupt...\n");
        dma_transfer_done_handler(4);
        break;
      case 121:
        printf("\nDMA CH5: Interrupt...\n");
        dma_transfer_done_handler(5);
        break;
      case 117:
        printf("\nDMA CH6: Interrupt...\n");
        dma_transfer_done_handler(6);
        break;
      case 105:
        printf("\nDMA CH7: Interrupt...\n");
        dma_transfer_done_handler(7);
        break;
      default:
        printf("UNKNOWN external interruption!\n");
        break;
      }
    }
  }
}

