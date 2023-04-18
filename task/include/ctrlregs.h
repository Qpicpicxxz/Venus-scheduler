#ifndef __TASK_CTRLREGS_H_
#define __TASK_CTRLREGS_H_

/* Venus Block Control Register Map  [PPT.70] */
#define VENUSBLOCK_CFGREG_OFFSET       0x00  // Venus Block Configuration Register
#define VENUSBLOCK_INTSTATUSREG_OFFSET 0x08  // Venus Block Interrupt Status Register
#define VENUSBLOCK_INTCLEARREG_OFFSET  0x10  // Venus Block Interrupt Clear Register
#define VENUSBLOCK_PROGRESSREG_OFFSET  0x18  // Task program may write this reg to present the progress of the task processing
#define VENUSBLOCK_RUNTIMEREG_OFFSET   0x20  // Indicating how many clock cycles hav passed since VenusBlcok soft reset asserted
#define VENUSBLOCK_NUMRETREG_OFFSET    0x28  // Number of return variables and vectors

/* i's range is 0 ~ 15 */
#define VENUSBLOCK_RETADDRREG_OFFSET(i) (0x30 + (i * 0x10))
#define VENUSBLOCK_RETLENREG_OFFSET(i)  (0x38 + (i * 0x10))

#define REG_RSTVAL_VENUSBLOCK_CFGREG       0x00000000
#define REG_RSTVAL_VENUSBLOCK_INTSTATUSREG 0x00000000
#define REG_RSTVAL_VENUSBLOCK_PROGRESSREG  0x00000000
#define REG_RSTVAL_VENUSBLOCK_RUNTIMEREG   0x00000000
#define REG_RSTVAL_VENUSBLOCK_NUMRETREG    0x00000000
#define REG_RSTVAL_VENUSBLOCK_RETADDRREG   0x00000000
#define REG_RSTVAL_VENUSBLOCK_RETLENREG    0x00000000

/* In VenusBlock_CfgReg */
#define SOFT_RST_CTRL(i) (i << 0)  // VenusBlock soft reset controller
#define WAIT_WB_FLAG(i)  (i << 1)  // VenusBlock Waiting for Write Back Flag

/* In VenusBlock_IntStatusReg */
#define EXEC_COMPLETE_IRQ(i) (i << 0)  // VenusBlock exec complete interrupt
#define DEBUG_UART_IRQ(i)    (i << 7)  // VenusBlock debug uart interrupt

#endif /* __TASK_CTRLREGS_H_ */

