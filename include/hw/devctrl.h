#ifndef __HW_DEVCTRL_H_
#define __HW_DEVCTRL_H_

/* [PPT.69]  Venus DevCtrl Module Register address offset and initial values */
/* Update: [P.115] Soc DevCtrl Register Map (Multi-level) */
#define VENUS_SCHEDULER_CORE_RST_OFFSET         0x00
#define VENUS_AHB_DEV_RST_OFFSET                0x04
#define VENUS_APB_DEV_RST_OFFSET                0x08
#define VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(n) (0x10 + ((n)*0x04))

#define VENUS_SCHEDULER_EN  (1 << 0)
#define VENUS_AHB_BUS_EN    (1 << 31)
#define VENUS_DMA_EN        (1 << 0)
#define VENUS_AHB_DMA_EN    0x80000001
#define VENUS_DEBUG_UART_EN (1 << 0)

#define CLUSTER_AXI_BUS_EN       (1 << 31)
#define VENUS_BLOCK_EN(K)        (1 << K)
#define CLUSTER_AXI_BLOCK_ALL_EN 0x8000ffff

#define VENUS_SCHEDULER_CORE_RST (VENUS_SCHEDULER_CORE_RST_OFFSET / 4)
#define VENUS_AHB_DEV_RST        (VENUS_AHB_DEV_RST_OFFSET / 4)
#define VENUS_APB_DEV_RST        (VENUS_APB_DEV_RST_OFFSET / 4)
#define VENUS_CLUSTER_DEV_RST_CLUSTER              \
  { (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[0] / 4), \
    (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[1] / 4), \
    (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[2] / 4), \
    (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[3] / 4), \
    (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[4] / 4), \
    (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[5] / 4), \
    (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[6] / 4), \
    (VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER[7] / 4) }

#define REG_RSTVAL_VENUS_SCHEDULER_CORE_RST 0x0u
#define REG_RSTVAL_VENUS_AHB_DEV_RST        0x0u
#define REG_RSTVAL_VENUS_APB_DEV_RST        0x0u
#define REG_RSTVAL_VENUS_CLUSTER_DEV_RST_CLUSTER \
  { 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u }

#endif /* __HW_DEVCTRL_H_ */

