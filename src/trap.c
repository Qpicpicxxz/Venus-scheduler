/* Exception Control Flow(ECP) --> execption / interrupt --> trap */
/*
  1. trap initialize
  2. top half of trap: hardware would complete
  -- set pc to be mtvec
  -- exception: mepc = pc
  -- interrupt: mepc = pc + 4
  3. bottom half of trap: software should complete
  4. back to normal
*/
#include "os.h"
#include "block.h"

extern void trap_vector(void);
extern int uart_getc(void);
extern void block_recycle(block_f *block);
static block_f block1;
static block_f block2;
static block_f block3;
static block_f block4;
static block_f block5;
static block_f block6;
static block_f block7;
static block_f block8;
// block_recycle(*block1);

/*
    set the trap-vector base-address for machine-mode
    csrw mtvec, trap_vector
    <mtvec>: Machine Trap-Vector Base-Address
    -------------------------------------------
    |         BASE(MXLEN-2)        |  MODE(2) |
    -------------------------------------------
    BASE: aligned to 4-byte boundary
          the base address of trap entry function
    MODE: -0:   all exceptions set pc to BASE
          -1:   asynchronous interrupts set pc to BASE + 4 × cause
          -2/3: reserved
*/
void trap_init() {
  /*
    An assemble code entry address label
    identify trap status jump to this processor
  */
  w_mtvec((reg_t)trap_vector);

  int hart = r_tp();
  *(uint32_t *)PLIC_PRIORITY(UART0_IRQ) = 1;
  // enalbe UART0
  *(uint32_t *)PLIC_MENABLE(hart) = (1 << UART0_IRQ);
  // set priority threshold for UART0
  // if we change it to 10, then the system would not handle UART IRQ
  *(uint32_t *)PLIC_MTHRESHOLD(hart) = 0;

  /* Enable machine-mode external interrupts. */
  // first read <mie>, then OR certain bit, and write back
  w_mie(r_mie() | MIE_MEIE);

  /* enable machine-mode global interrupts. */
  w_mstatus(r_mstatus() | MSTATUS_MIE);
}

int catch_keyboard() { return uart_getc(); }
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

/*
  In switch.S:
      a0 --> mepc  a1 --> mcause
  <mepc>: Machine Exception Program Counter Register
  <mcause>: Machine Cause Register

  When trap happend, pc will be replaced with <mtvec> it sets
  and hart will set <mepc> to be the current or next instruction address
  when we use <mret>, it could jump back to normal control flow.

  The upper 1 bit in mcause represent the trap reason:
    -0: exception
    -1: interrupt
    else bit: to identify specific interrupt/exception category
  <mtval>: Machine Trap Value Register,
           other supplementary information to identify exception
*/
reg_t trap_handler(reg_t epc, reg_t cause) {
  reg_t return_pc = epc;
  reg_t cause_code = cause & 0xfff; // catch the highest order

  // if highest order = 1, then it's an interrupt
  if (cause & 0x80000000) {
    /* Asynchronous trap - interrupt */
    /*
      1. local interrupt
        - software interrupt (0-3)
        - timer interrupt    (4-7)
      2. global interrupt
        - externel interrupt (8-11)
      <mie>: Machine Interrupt Enable  (write)
      <mip>: Machine Interrupt Pending (read)
        -0: close
        -1: open
    */
    // cause_code defined in Table 3.6: mcause values after trap
    switch (cause_code) {
    /* Software interruption */
    case 3:
      break;
    /* Timer interruption */
    case 7:
      break;
    /* External interruption */
    case 11:
      external_interrupt_handler();
      break;
    /* Add interruption type here */
    default:
      break;
    }
    // else highest order = 0, then it's an exception
  } else {
    /* Synchronous trap - exception */
    printf("Exception code = %d\n", cause_code);
    // TODO: implement all the exception warning
    while (1) {
    };
    /*
      panic is a dead loop
      because exception will back to where it trigger
      so without any other operation, it would trigge it again
      to avoid this happen
      we process a dead loop to prevent it return back
    */
    // panic("OOPS! What can I do!");
    //  we can change return address by ourself
    return_pc += 4;
  }

  return return_pc;
}

void trap_test() {
  uart_puts("\nSCHEDULER: Testing trap function...\n");
  /*
    raise a memory access failure exception
    synchronous exception code = 7
    store/AMO access fault
    then hart would complete top-trap handler
  */
  *(int *)0x00000000 = 100;

  /*
    Synchronous exception code = 5
    Load access fault
  */
  // int a = *(int *)0x00000000;

  // if we make return_pc + 4, it would return back to this instruction
  uart_puts("Return back from trap...\n");
}

