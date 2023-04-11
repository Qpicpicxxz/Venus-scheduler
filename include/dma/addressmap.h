
#ifndef __DMA_ADDRESS_H__
#define __DMA_ADDRESS_H__

/* Venus SoC Address Map */
#define SOC_OCRAM_ADDR           0x00000000UL
/* ----------------------------------------*/
#define SOC_BOOTROM_ADDR         0x00010000UL
/* ----------------------------------------*/
#define SOC_APB_BASE_ADDR        0x02000000UL

#define SOC_CCM_ADDR             0x02000000UL
#define SOC_DEVCTRL_ADDR         0x02000800UL
#define SOC_DDRPHY_CFG_ADDR      0x02004000UL
#define SOC_DDRC_CFG_ADDR        0x02008000UL

/* ----------------------------------------*/
#define SOC_AHB_BASE_ADDR        0x04000000UL
/* ----------------------------------------*/

#define VENUS_AXI4_BASE_ADDR     0x10000000UL

#define VENUS_DMAC_ADDR          0x10000000UL
#define VENUS_CCM_ADDR           0x10010000UL
#define VENUS_DEVCTRL_ADDR       0x10010800UL

#define VENUS_CLUSTER_ADDR       0x20000000UL

#define CLUSTER_0_OFFSET         0x00000000UL
#define CLUSTER_1_OFFSET         0x04000000UL
#define CLUSTER_2_OFFSET         0x08000000UL
#define CLUSTER_3_OFFSET         0x0C000000UL
#define CLUSTER_4_OFFSET         0x10000000UL
#define CLUSTER_5_OFFSET         0x14000000UL
#define CLUSTER_6_OFFSET         0x18000000UL
#define CLUSTER_7_OFFSET         0x1C000000UL

#define BLOCK_0_OFFSET           0x00000000UL
#define BLOCK_1_OFFSET           0x00400000UL
#define BLOCK_2_OFFSET           0x00800000UL
#define BLOCK_3_OFFSET           0x00C00000UL
#define BLOCK_4_OFFSET           0x01000000UL
#define BLOCK_5_OFFSET           0x01400000UL
#define BLOCK_6_OFFSET           0x01800000UL
#define BLOCK_7_OFFSET           0x01C00000UL
#define BLOCK_8_OFFSET           0x02000000UL
#define BLOCK_9_OFFSET           0x02400000UL
#define BLOCK_10_OFFSET          0x02800000UL
#define BLOCK_11_OFFSET          0x02C00000UL
#define BLOCK_12_OFFSET          0x03000000UL
#define BLOCK_13_OFFSET          0x03400000UL
#define BLOCK_14_OFFSET          0x03800000UL
#define BLOCK_15_OFFSET          0x03C00000UL

/* ----------------------------------------*/
#define SOC_RESERVED_S6_ADDR     0x40000000UL
#define SOC_RESERVED_S7_ADDR     0x60000000UL
#define SOC_DDR_ADDR             0x80000000UL
/* ----------------------------------------*/

#endif /* __DMA_ADDRESS_H__ */
