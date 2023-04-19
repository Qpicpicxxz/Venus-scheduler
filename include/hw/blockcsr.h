#ifndef __HW_BLOCKCSR_H_
#define __HW_BLOCKCSR_H_

/* Venus Block Control Register Map */
#define VENUSBLOCK_CFGREG_OFFSET       0x00
#define VENUSBLOCK_INTSTATUSREG_OFFSET 0x08
#define VENUSBLOCK_INTCLEARREG_OFFSET  0x10
#define VENUSBLOCK_PROGRESSREG_OFFSET  0x18
#define VENUSBLOCK_RUNTIMEREG_OFFSET   0x20
#define VENUSBLOCK_NUMRETREG_OFFSET    0X28

#define VenusBlock_RetAddr_BASE 0x30
#define VenusBlock_RetLen_BASE  0x38

/* idx's range is 0 ~ 15 */
#define VenusBlock_RetAddr(idx) (VenusBlock_RetAddr_BASE + (idx * 0x10))
#define VenusBlock_RetLen(idx)  (VenusBlock_RetLen_BASE + (idx * 0x10))

#endif /* __HW_BLOCKCSR_H_ */

