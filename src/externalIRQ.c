#include "os.h"
#include "block.h"

// a handler to remove keyboard interrupt in uart.c
extern int uart_getc(void);

// a handler to recycle idle blocks and pass the computing result to the successor
extern void block_recycle(block_f *block);

// allocate all the block flag's memory space
static block_f block1;
static block_f block2;
static block_f block3;
static block_f block4;
static block_f block5;
static block_f block6;
static block_f block7;
static block_f block8;

// this handler could catch the press button and remove this interrupt
int catch_keyboard() { return uart_getc(); }

// to handler external interrupt
void external_interrupt_handler() {
  /*
    I use kayboard interrupt to simulate block interrupt,
    so when I press a key, it would rise a interrupt,
    and uart_getc() function acts as to clear this interrupt
  */
  int c = catch_keyboard();
  /*
    I use different key to represent different block's signal
    suppose there are 8 VENUS blocks
    ------------------------------------------
      1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |
    ------------------------------------------
     49 | 50 | 51 | 52 | 53 | 54 | 55 | 56 |
    ------------------------------------------
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
    printf("UNKNOWN external interruption!");
    break;
  }
}

