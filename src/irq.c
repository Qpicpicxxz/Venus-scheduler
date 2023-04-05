#include "common.h"
/*
 *   +-----+-------------------------------------+
 *   | IRQ |         Interrupt Source            |
 *   +-----+-------------------------------------+
 *   |  0  | Timer Interrupt                     | 0
 *   +-----+-------------------------------------+
 *   |  1  | EBREAK/ECALL or Illegal Instruction | 2
 *   +-----+-------------------------------------+
 *   |  2  | BUS Error (Unalign Memory Access)   | 4
 *   +-----+-------------------------------------+
 *   |  3  | DMA Interrupt                       | 8
 *   +-----+-------------------------------------+
 *   |  4  | BLOCK Interrupt                     | 16
 *   +-----+-------------------------------------+
 *   | ... | RESERVED                            |
 *   +-----+-------------------------------------+
 */

/* task_callback.c */
extern void block_handler(uint32_t block_index);

/* In start.S: a0 --> *regs  a1 --> cause(q1) */
void irq_handler(reg_t* regs, reg_t cause) {
  if ((cause & 6) != 0) {
    uint32_t pc    = (regs[0] & 1) ? regs[0] - 3 : regs[0] - 4;
    uint32_t instr = *(uint32_t*)pc;

    if ((cause & 2) != 0) {
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

    if ((cause & 4) != 0) {
      // print_str("Bus error in Instruction at 0x");
      // print_hex(pc, 8);
      // print_str(": 0x");
      // print_hex(instr, ((instr & 3) == 3) ? 8 : 4);
      // print_str("\n");
    }
  }

  if ((cause & 8) != 0) {
    // DMA Interrupt
  }

  if ((cause & 16) != 0) {
    // need q1 = 0000_0000_0001_0000
    // Block Interrupt
    block_handler(0);
  }
}

