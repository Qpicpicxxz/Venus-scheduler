#ifndef __BLOCKCSR_H_
#define __BLOCKCSR_H_

/* Venus Block Control Register Map */
#define VenusBlock_CfgReg       0x00
#define VenusBlock_IntStatusReg 0x08
#define VenusBlock_IntClearReg  0x10
#define VenusBlock_ProgressReg  0x18
#define VenusBlock_RunTimeReg   0x20
#define VenusBlock_NumRetReg    0X28

#define VenusBlock_RetAddr_BASE 0x30
#define VenusBlock_RetLen_BASE  0x38

/* idx's range is 0 ~ 15 */
#define VenusBlock_RetAddr(idx) (VenusBlock_RetAddr_BASE + (idx * 0x10))
#define VenusBlock_RetLen(idx)  (VenusBlock_RetLen_BASE + (idx * 0x10))

#endif /* __BLOCKCSR_H_ */

