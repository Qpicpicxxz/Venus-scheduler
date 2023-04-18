#include "common.h"

void job_done_callback(void) {
  // 向VenusBlock_IntStatusReg寄存器的bit[0]写入1，使能本block的"task执行完毕中断"
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_INTSTATUSREG_OFFSET, 1);
  // 向VenusBlock_CfgReg寄存器bit[1]写入1，使能本block的"Waiting for Write Back Flag"
  // 向VenusBlock_CfgReg寄存器bit[0]写入0，使能本模块的soft reset，从而实现pico的自关闭
  uint32_t cfg_reg = SOFT_RST_CTRL(0) | WAIT_WB_FLAG(1);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_CFGREG_OFFSET, cfg_reg);
}
