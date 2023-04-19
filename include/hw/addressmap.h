#ifndef __HW_ADDRESSMAP_H__
#define __HW_ADDRESSMAP_H__

/* Venus SoC Address Map */
#define SOC_OCRAM_ADDR    0x00000000UL
#define SOC_BOOTROM_ADDR  0x00010000UL
#define SOC_APB_BASE_ADDR 0x02000000UL

#define SOC_CCM_ADDR        0x02000000UL
#define SOC_DEVCTRL_ADDR    0x02000800UL
#define SOC_UART0_ADDR      0x02001000UL
#define SOC_DDRPHY_CFG_ADDR 0x02004000UL
#define SOC_DDRC_CFG_ADDR   0x02008000UL

#define SOC_AHB_BASE_ADDR 0x04000000UL

#define VENUS_AXI4_BASE_ADDR 0x10000000UL

#define VENUS_DMAC_ADDR       0x10000000UL
#define VENUS_CCM_ADDR        0x10010000UL
#define VENUS_DEVCTRL_ADDR    0x10010800UL
#define VENUS_DEBUG_UART_ADDR 0x10011000UL

#define VENUS_CLUSTER_ADDR 0x20000000UL

#define CLUSTER_OFFSET(N) (N << 26)

#define BLOCK_OFFSET(K) (K << 22)

#define BLOCK_ISPM_OFFSET      0x00000000UL  // 1MB for each - Instruction
#define BLOCK_SDSPM_OFFSET     0x00100000UL  // 1MB for each - Scalar
#define BLOCK_VDSPM_OFFSET     0x00200000UL  // 1MB for each - Vector
#define BLOCK_CTRLREGS_OFFSET  0x00300000UL
#define BLOCK_DEBUGUART_OFFSET 0x00300200UL

#define SOC_RESERVED_S6_ADDR 0x40000000UL
#define SOC_RESERVED_S7_ADDR 0x60000000UL
#define SOC_DDR_ADDR         0x80000000UL

/* From the SoC's perspective:                                                         \
 *  Nth cluster, Kth block (0<=N<=7, 0<=K<=15)                                         \
 *  I-SPM = VENUS_CLUSTER_ADDR + CLUSTER_N_OFFSET + BLOCK_K_OFFSET + BLOCK_ISPM_OFFSET \
 */

#endif /* __HW_ADDRESSMAP_H__ */

