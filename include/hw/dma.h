#ifndef __HW_DMA_H__
#define __HW_DMA_H__

#define DMAC_ID_REG_OFFSET                   0x000
#define DMAC_COMP_VER_REG_OFFSET             0x008
#define DMAC_CFG_REG_OFFSET                  0x010
#define DMAC_CH_EN_REG_OFFSET                0x018
#define DMAC_INTR_STATUS_REG_OFFSET          0x030
#define DMAC_COMMON_INTR_CLEAR_REG_OFFSET    0x038
#define DMAC_COMMON_STATUS_ENABLE_REG_OFFSET 0x040
#define DMAC_COMMON_SIGNAL_ENABLE_REG_OFFSET 0x048
#define DMAC_COMMON_INTR_STATUS_REG_OFFSET   0x050
#define DMAC_RST_REG_REG_OFFSET              0x058

#define DMAC_CH_SAR_REG_OFFSET_CH(ch_num)                (0x100 * ((ch_num) + 1) + 0x000)
#define DMAC_CH_DAR_REG_OFFSET_CH(ch_num)                (0x100 * ((ch_num) + 1) + 0x008)
#define DMAC_CH_BLOCK_TS_REG_OFFSET_CH(ch_num)           (0x100 * ((ch_num) + 1) + 0x010)
#define DMAC_CH_CTL_REG_OFFSET_CH(ch_num)                (0x100 * ((ch_num) + 1) + 0x018)
#define DMAC_CH_CFG_REG_OFFSET_CH(ch_num)                (0x100 * ((ch_num) + 1) + 0x020)
#define DMAC_CH_LLP_REG_OFFSET_CH(ch_num)                (0x100 * ((ch_num) + 1) + 0x028)
#define DMAC_CH_STATUS_REG_OFFSET_CH(ch_num)             (0x100 * ((ch_num) + 1) + 0x030)
#define DMAC_CH_SWHSSRC_REQ_REG_OFFSET_CH(ch_num)        (0x100 * ((ch_num) + 1) + 0x038)
#define DMAC_CH_SWHSDST_REQ_REG_OFFSET_CH(ch_num)        (0x100 * ((ch_num) + 1) + 0x040)
#define DMAC_CH_BLK_TFR_RESUME_REQ_REG_OFFSET_CH(ch_num) (0x100 * ((ch_num) + 1) + 0x048)
#define DMAC_CH_AXI_ID_REG_OFFSET_CH(ch_num)             (0x100 * ((ch_num) + 1) + 0x050)
#define DMAC_CH_AXI_QOS_REG_OFFSET_CH(ch_num)            (0x100 * ((ch_num) + 1) + 0x058)
#define DMAC_CH_SSTAT_REG_OFFSET_CH(ch_num)              (0x100 * ((ch_num) + 1) + 0x060)
#define DMAC_CH_DSTAT_REG_OFFSET_CH(ch_num)              (0x100 * ((ch_num) + 1) + 0x068)
#define DMAC_CH_STATAR_REG_OFFSET_CH(ch_num)             (0x100 * ((ch_num) + 1) + 0x070)
#define DMAC_CH_DSTATAR_REG_OFFSET_CH(ch_num)            (0x100 * ((ch_num) + 1) + 0x078)
#define DMAC_CH_INTR_STATUS_ENABLE_REG_OFFSET_CH(ch_num) (0x100 * ((ch_num) + 1) + 0x080)
#define DMAC_CH_INTR_STATUS_REG_OFFSET_CH(ch_num)        (0x100 * ((ch_num) + 1) + 0x088)
#define DMAC_CH_INTR_SIGNAL_ENABLE_REG_OFFSET_CH(ch_num) (0x100 * ((ch_num) + 1) + 0x090)
#define DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(ch_num)         (0x100 * ((ch_num) + 1) + 0x098)

// #define DMA_MAX_TRANSFER_LENGTH (2048 * 8) // previous verison - 16k
#define DMA_MAX_TRANSFER_LENGTH (4 * 1024 * 1024)  // 4MB
#define DMAC_NUMBER_OF_CHANNELS 1
#define EN_SOFT_RST             0x1

#endif /* __HW_DMA_H__ */

