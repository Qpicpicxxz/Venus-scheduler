#ifndef __DMA_AXI_H__
#define __DMA_AXI_H__

/*
 * Destination / Source Transfer Width [P.156]
 * Used in CHx_CTL.SRC_TR_WIDTH / CHx_CTL.DST_TR_WIDTH
 * size is 8 ~ 512 bits
 */
#define AXI4_BURST_SIZE_1_BYTE  0b000
#define AXI4_BURST_SIZE_2_BYTE  0b001
#define AXI4_BURST_SIZE_4_BYTE  0b010
#define AXI4_BURST_SIZE_8_BYTE  0b011
#define AXI4_BURST_SIZE_16_BYTE 0b100
#define AXI4_BURST_SIZE_32_BYTE 0b101
#define AXI4_BURST_SIZE_64_BYTE 0b110

#endif /* __DMA_AXI_H__ */

