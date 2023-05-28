#include "block.h"
#include "common.h"
#include "hal.h"
#include "hw/addressmap.h"
#include "hw/blockcsr.h"
#include "hw/config.h"
#include "ulib.h"

extern int printf(const char* s, ...);
extern void DMAC_interrupt_handler(void);
extern block_t block_stru[MAX_NUM_CLUSTERS][MAX_NUM_BLOCKS];
extern void block_handler(block_t* n_block);

void IRQ_CLUSTER0_handler(void) {
  for (int i = 0; i < MAX_NUM_BLOCKS; i++) {
    // 向产生task运行完毕中断的block的 Control Registers 中的 VenusBlock_IntClearReg寄存器中的bit[0]写清除VENUS BLOCK中断
    // 当CLUSTER中所有BLOCK的中断全部清除时，venus_cluster中断自动清除
    if (READ_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(0) + BLOCK_OFFSET(i) + BLOCK_CTRLREGS_OFFSET, VENUSBLOCK_INTSTATUSREG_OFFSET)) {
      WRITE_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(0) + BLOCK_OFFSET(i) + BLOCK_CTRLREGS_OFFSET, VENUSBLOCK_INTCLEARREG_OFFSET, 1);
      block_handler(&block_stru[0][i]);
      return;
    }
  }
}
void IRQ_CLUSTER1_handler(void) {
  // printf("CLUSTER 1 Interrupt! $stop\n");
  for (int i = 0; i < MAX_NUM_BLOCKS; i++) {
    // 向产生task运行完毕中断的block的 Control Registers 中的 VenusBlock_IntClearReg寄存器中的bit[0]写清除VENUS BLOCK中断
    // 当CLUSTER中所有BLOCK的中断全部清除时，venus_cluster中断自动清除
    if (READ_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(1) + BLOCK_OFFSET(i) + BLOCK_CTRLREGS_OFFSET, VENUSBLOCK_INTSTATUSREG_OFFSET)) {
      WRITE_BURST_32(VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(1) + BLOCK_OFFSET(i) + BLOCK_CTRLREGS_OFFSET, VENUSBLOCK_INTCLEARREG_OFFSET, 1);
      block_handler(&block_stru[1][i]);
      return;
    }
  }
}
void IRQ_CLUSTER2_handler(void) {
  printf("CLUSTER 2 Interrupt! $stop\n");
}
void IRQ_CLUSTER3_handler(void) {
  printf("CLUSTER 3 Interrupt! $stop\n");
}
void IRQ_CLUSTER4_handler(void) {
  printf("CLUSTER 4 Interrupt! $stop\n");
}
void IRQ_CLUSTER5_handler(void) {
  printf("CLUSTER 5 Interrupt! $stop\n");
}
void IRQ_CLUSTER6_handler(void) {
  printf("CLUSTER 6 Interrupt! $stop\n");
}
void IRQ_CLUSTER7_handler(void) {
  printf("CLUSTER 7 Interrupt! $stop\n");
}

void IRQ_DMA_handler(void) {
  DMAC_interrupt_handler();
}

/* global variable to save irq mask values */
uint32_t irq_mask;
static VoidFunc irq_callback[32];

void set_handler(uint32_t irq, VoidFunc callback) {
  irq_callback[irq] = callback;
}

void enable_irq(uint32_t irq) {
  irq_mask = EN_Interrupts(1 << irq);
}

void disable_irq(uint32_t irq) {
  irq_mask = DIS_Interrupts(1 << irq);
}

void mask_irq(uint32_t mask) {
  __asm__ __volatile__("mv a4, a0");
  // maskirq a5, a4
  __asm__ __volatile__(".word(0x0607678b)");
  return;
}

/* In start.S: a0 --> *regs  a1 --> cause(q1) */
uint32_t* irq_handler(reg_t* regs, reg_t cause) {

  // irq_mask = irq_mask | cause;
  // mask_irq(irq_mask);
  if ((cause & (1 << PICO_IRQ_BADINSTR)) || (cause & (1 << PICO_IRQ_MEMERROR))) {
    uint32_t pc    = (regs[0] & 1) ? regs[0] - 3 : regs[0] - 4;
    uint32_t instr = *(uint32_t*)pc;

    if (cause & (1 << PICO_IRQ_BADINSTR)) {
      if (instr == 0x00100073 || instr == 0x9002) {
        printf("[SCHEDULER] EBREAK instruction at %p $stop\n", pc);
      } else {
        printf("[SCHEDULER] Illegal instruction at %p $stop\n", pc);
      }
    }

    if (cause & (1 << PICO_IRQ_MEMERROR)) {
      printf("[SCHEDULER] Bus error in Instruction at %p $stop\n", pc);
    }
  }

  if (cause & (1 << VENUS_IRQ_CLUSTER_0)) {
    irq_callback[VENUS_IRQ_CLUSTER_0]();
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_1)) {
    irq_callback[VENUS_IRQ_CLUSTER_1]();
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_2)) {
    irq_callback[VENUS_IRQ_CLUSTER_2]();
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_3)) {
    irq_callback[VENUS_IRQ_CLUSTER_3]();
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_4)) {
    irq_callback[VENUS_IRQ_CLUSTER_4]();
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_5)) {
    irq_callback[VENUS_IRQ_CLUSTER_5]();
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_6)) {
    irq_callback[VENUS_IRQ_CLUSTER_6]();
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_7)) {
    irq_callback[VENUS_IRQ_CLUSTER_7]();
  }

  if (cause & (1 << VENUS_IRQ_DMA)) {
    // uart_puts("DMA IRQ!\n");
    irq_callback[VENUS_IRQ_DMA]();
    // DMA Interrupt
  }

  // irq_mask = irq_mask & (~cause); // unmask the finished interrupt
  // mask_irq(irq_mask);
  return regs;
}

void irq_init(void) {
  set_handler(VENUS_IRQ_CLUSTER_0, IRQ_CLUSTER0_handler);
  set_handler(VENUS_IRQ_CLUSTER_1, IRQ_CLUSTER1_handler);
  set_handler(VENUS_IRQ_CLUSTER_2, IRQ_CLUSTER2_handler);
  set_handler(VENUS_IRQ_CLUSTER_3, IRQ_CLUSTER3_handler);
  set_handler(VENUS_IRQ_CLUSTER_4, IRQ_CLUSTER4_handler);
  set_handler(VENUS_IRQ_CLUSTER_5, IRQ_CLUSTER5_handler);
  set_handler(VENUS_IRQ_CLUSTER_6, IRQ_CLUSTER6_handler);
  set_handler(VENUS_IRQ_CLUSTER_7, IRQ_CLUSTER7_handler);
  set_handler(VENUS_IRQ_DMA, IRQ_DMA_handler);
  enable_irq(PICO_IRQ_BADINSTR);
  enable_irq(PICO_IRQ_MEMERROR);
  enable_irq(VENUS_IRQ_CLUSTER_0);
  enable_irq(VENUS_IRQ_CLUSTER_1);
  enable_irq(VENUS_IRQ_CLUSTER_2);
  enable_irq(VENUS_IRQ_CLUSTER_3);
  enable_irq(VENUS_IRQ_CLUSTER_4);
  enable_irq(VENUS_IRQ_CLUSTER_5);
  enable_irq(VENUS_IRQ_CLUSTER_6);
  enable_irq(VENUS_IRQ_CLUSTER_7);
  enable_irq(VENUS_IRQ_DMA);
}
