#ifndef __BLOCKCSR_H_
#define __BLOCKCSR_H_

/* Venus Block Control Register Map */
#define VENUSBLOCK_CFGREG_OFFSET       0x00
#define VENUSBLOCK_INTSTATUSREG_OFFSET 0x08
#define VENUSBLOCK_INTCLEARREG_OFFSET  0x10
#define VENUSBLOCK_PROGRESSREG_OFFSET  0x18
#define VENUSBLOCK_RUNTIMEREG_OFFSET   0x20
#define VENUSBLOCK_NUMRETREG_OFFSET    0x28

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

#endif /* __BLOCKCSR_H_ */

