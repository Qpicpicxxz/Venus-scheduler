#ifndef __HAL_H__
#define __HAL_H__

#include "types.h"

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

#define IRQ_TIMER 0
#define IRQ_BADINSTR 1
#define IRQ_MEMERROR 2
#define IRQ_DMA 3
#define IRQ_BLOCK 4

typedef void(*VoidFunc)(void);

/* READ ONLY */
// extern unsigned int _irq_pending;

/* irq.c */
void set_handler(uint32_t irq,VoidFunc callback);
void enable_irq(uint32_t irq);
void disable_irq(uint32_t irq);

/* hal.S */
uint32_t EN_Interrupts(uint32_t mask);
uint32_t DIS_Interrupts(uint32_t mask);

/*static inline unsigned int irq_pending(void){
  return _irq_pending;
}*/


#endif /* __HAL_H__ */
