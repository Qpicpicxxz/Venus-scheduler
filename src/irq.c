#include "common.h"
#include "hal.h"

/* task_callback.c */
extern void block_handler(uint32_t block_index);

void IRQ_blockhandler(void) {
  block_handler(1);
}

static VoidFunc irq_callback[32];

void set_handler(uint32_t irq, VoidFunc callback) {
  irq_callback[irq] = callback;
}

void enable_irq(uint32_t irq) {
  EN_Interrupts(1 << irq);
}

void disable_irq(uint32_t irq) {
  DIS_Interrupts(1 << irq);
}

/* In start.S: a0 --> *regs  a1 --> cause(q1) */
void irq_handler(reg_t* regs, reg_t cause) {
  if ((cause & (1 << IRQ_BADINSTR)) || (cause & (1 << IRQ_MEMERROR))) {
    uint32_t pc    = (regs[0] & 1) ? regs[0] - 3 : regs[0] - 4;
    uint32_t instr = *(uint32_t*)pc;

    if (cause & (1 << IRQ_BADINSTR)) {
      if (instr == 0x00100073 || instr == 0x9002) {
        // print_str("EBREAK instruction at 0x");
        // print_hex(pc, 8);
        // print_str("\n");
      } else {
        // print_str("Illegal Instruction at 0x");
        // print_hex(pc, 8);
        // print_str(": 0x");
        // print_hex(instr, ((instr & 3) == 3) ? 8 : 4);
        // print_str("\n");
      }
    }

    if (cause & (1 << IRQ_MEMERROR)) {
      // print_str("Bus error in Instruction at 0x");
      // print_hex(pc, 8);
      // print_str(": 0x");
      // print_hex(instr, ((instr & 3) == 3) ? 8 : 4);
      // print_str("\n");
    }
  }

  if (cause & (1 << IRQ_DMA)) {
    // DMA Interrupt
  }

  if (cause & (1 << IRQ_BLOCK)) {
    // need q1 = 0000_0000_0001_0000
    // Block Interrupt
    irq_callback[IRQ_BLOCK]();
  }
}

void irq_init(void) {
  set_handler(IRQ_BLOCK, IRQ_blockhandler);
  enable_irq(IRQ_BLOCK);
}

