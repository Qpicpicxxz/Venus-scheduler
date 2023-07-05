#include "cluster.h"
#include "common.h"
#include "hal.h"
#include "hw/addressmap.h"
#include "hw/clustercsr.h"
#include "hw/config.h"
#include "ulib.h"

extern int printf(const char* s, ...);
extern void DMAC_interrupt_handler(void);
extern cluster_t cluster_stru[MAX_NUM_CLUSTERS];
extern void cluster_handler(cluster_t* n_cluster);

/* global variable to save irq mask values */
uint32_t irq_mask;

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

  if (cause & (1 << VENUS_IRQ_DMA)) {
    DMAC_interrupt_handler();
  }

  if (cause & (1 << VENUS_IRQ_CLUSTER_0)) {
    // clear cluster's interrupt
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(0), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[0]);
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_1)) {
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(1), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[1]);
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_2)) {
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(2), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[2]);
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_3)) {
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(3), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[3]);
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_4)) {
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(4), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[4]);
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_5)) {
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(5), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[5]);
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_6)) {
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(6), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[6]);
  }
  if (cause & (1 << VENUS_IRQ_CLUSTER_7)) {
    WRITE_BURST_32(VENUS_CLUSTER_CTRLREG_ADDR(7), VENUSCLUSTER_INTCLEARREG_OFFSET, 1);
    cluster_handler(&cluster_stru[7]);
  }

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

  return regs;
}

void irq_init(void) {
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

