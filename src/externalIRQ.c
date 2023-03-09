#include "os.h"
#include "block.h"


/* Function: remove keyboard interrupt in uart.c */
extern int uart_getc(void);

/* 
 * Function:
 * 	1. recycle idle blocks
 * 	2. pass the computing result to the successor
 */
extern void block_recycle(block_f *block);

/* define all the block's flag --- allocate their memory space */
static block_f block1;
static block_f block2;
static block_f block3;
static block_f block4;
static block_f block5;
static block_f block6;
static block_f block7;
static block_f block8;


void init_block_spm() {
	block1.spm_addr = SPM1_ADDR;
	block2.spm_addr = SPM2_ADDR;
	block3.spm_addr = SPM3_ADDR;
	block4.spm_addr = SPM4_ADDR;
	block5.spm_addr = SPM5_ADDR;
	block6.spm_addr = SPM6_ADDR;
	block7.spm_addr = SPM7_ADDR;
	block8.spm_addr = SPM8_ADDR;
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
        block_recycle(&block1);
        break;
      case 50:
        printf("\nBLOCK 2: Interrupt...\n");
        block_recycle(&block2);
        break;
      case 51:
        printf("\nBLOCK 3: Interrupt...\n");
        block_recycle(&block3);
        break;
      case 52:
        printf("\nBLOCK 4: Interrupt...\n");
        block_recycle(&block4);
        break;
      case 53:
        printf("\nBLOCK 5: Interrupt...\n");
        block_recycle(&block5);
        break;
      case 54:
        printf("\nBLOCK 6: Interrupt...\n");
        block_recycle(&block6);
        break;
      case 55:
        printf("\nBLOCK 7: Interrupt...\n");
        block_recycle(&block7);
        break;
      case 56:
        printf("\nBLOCK 8: Interrupt...\n");
        block_recycle(&block8);
        break;
      default:
        printf("UNKNOWN external interruption!\n");
        break;
      }
    }
  }
}

