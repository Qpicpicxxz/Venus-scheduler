#ifndef __TASK_DEVICE_
#define __TASK_DEVICE_

/* [PPT.65] From the block's perspective */
#define BLOCK_ISPM      0x80000000UL  // 1MB - Instruction I-SPM
#define BLOCK_SDSPM     0x80100000UL  // 1MB - Scalar D-SPM
#define BLOCK_VDSPM     0x80200000UL  // 1MB - VRF D-SPM
#define BLOCK_CTRLREGS  0x80300000UL  // Control Registers
#define BLOCK_DEBUGUART 0x80300200UL  // Debug UART

#endif /* __TASK_DEVICE_ */
