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

extern void trap_vector(void);
extern void uart_isr(void);

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
}

void external_interrupt_handler() {

  int irq = plic_claim();

  if (irq == UART0_IRQ) {
    uart_isr();
  } else if (irq) {
    printf("unexpected interrupt irq = %d\n", irq);
  }

  if (irq) {
    plic_complete(irq);
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
    switch (cause_code) {
    case 3:
      uart_puts("software interruption!\n");
      break;
    case 7:
      uart_puts("timer interruption!\n");
      break;
    case 11:
      uart_puts("external interruption!\n");
      external_interrupt_handler();
      break;
    default:
      uart_puts("unknown async exception!\n");
      break;
    }
    // else highest order = 0, then it's an exception
  } else {
    /* Synchronous trap - exception */
    printf("Exception code = %d\n", cause_code);
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

