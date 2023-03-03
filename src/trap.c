/* Exception Control Flow(ECP) --> execption / interrupt --> trap */
/*
 * 1. trap initialize
 * 2. top half of trap: hardware would complete
 * -- set pc to be mtvec
 * -- exception: mepc = pc
 * -- interrupt: mepc = pc + 4
 * 3. bottom half of trap: software should complete
 * 4. back to normal
 */
#include "os.h"

// trap entry address in switch.S
extern void trap_vector(void);
// a external interrupt handler in externalIRQ.c
extern void external_interrupt_handler(void);

/*
 * set the trap-vector base-address for machine-mode
 * csrw mtvec, trap_vector
 * <mtvec>: Machine Trap-Vector Base-Address
 * -------------------------------------------
 * |         BASE(MXLEN-2)        |  MODE(2) |
 * -------------------------------------------
 *  BASE: aligned to 4-byte boundary
 *        the base address of trap entry function
 *  MODE: -0:   all exceptions set pc to BASE
 *        -1:   asynchronous interrupts set pc to BASE + 4 × cause
 *        -2/3: reserved
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
  /* set priority threshold for UART0
   * if we change it to 10, then the system would not handle UART IRQ
   */
  *(uint32_t *)PLIC_MTHRESHOLD(hart) = 0;
  /* Enable machine-mode external interrupts. */
  w_mie(r_mie() | MIE_MEIE);

  /* enable machine-mode global interrupts. */
  w_mstatus(r_mstatus() | MSTATUS_MIE);
}

/*
 * In switch.S:
 *   a0 --> mepc  a1 --> mcause
 * <mepc>: Machine Exception Program Counter Register
 * <mcause>: Machine Cause Register
 * When trap happend, pc will be replaced with <mtvec> it sets
 * and hart will set <mepc> to be the current or next instruction address
 * when we use <mret>, it could jump back to normal control flow.
 * The upper 1 bit in mcause represent the trap reason:
 * -0: exception
 * -1: interrupt
 * else bit: to identify specific interrupt/exception category
 * <mtval>: Machine Trap Value Register,
 *          other supplementary information to identify exception
 */
reg_t trap_handler(reg_t epc, reg_t cause) {
  reg_t return_pc = epc;
  reg_t cause_code = cause & 0xfff; // catch the highest order

  // if highest order = 1, then it's an interrupt
  if (cause & 0x80000000) {
    /* Asynchronous trap - interrupt */
    /*
     * 1. local interrupt
     *  - software interrupt (0-3)
     *  - timer interrupt    (4-7)
     * 2. global interrupt
     *  - externel interrupt (8-11)
     * <mie>: Machine Interrupt Enable  (write)
     * <mip>: Machine Interrupt Pending (read)
     *   -0: close
     *   -1: open
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
    switch (cause_code) {
    case 0:
      printf("Exception 0: Instruction address misaligned\n");
      break;
    case 1:
      printf("Exception 1: Instruction access fault\n");
      while(1){};
    case 2:
      printf("Exception 2: Illegal instruction\n");
      while(1){};
    case 3:
      printf("Exception 3: Breakpoint\n");
      while(1){};
    case 4:
      printf("Exception 4: Load address misaligned\n");
      while(1){};
    case 5:
      printf("Exception 5: Load access fault\n");
      while(1){};
    case 6:
      printf("Exception 6: Store/AMO address misaligned\n");
      while(1){};
    case 7:
      printf("Exception 7: Store/AMO access fault\n");
      while(1){};
    case 8:
      printf("Exception 8: Environment call from U-mode\n");
      while(1){};
    case 9:
      printf("Exception 9: Environment call from S-mode\n");
      while(1){};
    case 11:
      printf("Exception 11: Environment call from M-mode\n");
      while(1){};
    case 12:
      printf("Exception 12: Instruction page fault\n");
      while(1){};
    case 13:
      printf("Exception 13: Load page fault\n");
      while(1){};
    case 15:
      printf("Exception 15: Store/AMO page fault\n");
      while(1){};
    default:
      printf("Exception %d: Reserved\n", cause_code);
    }
    /*
     * panic is a dead loop
     * because exception will back to where it trigger
     * so without any other operation, it would trigge it again
     * to avoid this happen
     * we process a dead loop to prevent it return back
     */
    // panic("OOPS! What can I do!");
    //  we can change return address by ourself
    // return_pc += 4;
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

