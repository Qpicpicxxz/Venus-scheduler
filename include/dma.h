#ifndef __DMA_H_
#define __DMA_H_

#include "assert.h"
#include "hw/addressmap.h"
#include "hw/dma.h"
#include "lli.h"
#include "types.h"
#include "ulib.h"

#define CHANNEL_MASK   ((1 << DMAC_NUMBER_OF_CHANNELS) - 1)
#define BIT_PICK(N, i) ((N >> i) & 1)
#define ALL_1_RESET    0xffffffffffffffff
#define FREE_CHANNEL_WRONG_LABEL 999

/* dma tools */
extern lli_t* malloc_LLI(void);
extern void free_LLI(lli_t* ptr);

extern uint64_t destination_max_burst_length;
extern uint64_t source_max_burst_length;
extern uint32_t destination_transfer_width;
extern uint32_t source_transfer_width;

/* printf */
extern int printf(const char* s, ...);  // only understands %d, %x, %p, %s

/* DMA irq handlers */
inline uint32_t DMAC_get_free_channel(void) {
  uint64_t ch_en_reg = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_CH_EN_REG_OFFSET);
  // uint32_t free_channel_index;
  // while(READDATA64[(DMAC_NUMBER_OF_CHANNELS-1):0]=={(DMAC_NUMBER_OF_CHANNELS){1'b1}})
  while ((ch_en_reg & CHANNEL_MASK) == CHANNEL_MASK) {
    ch_en_reg = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_CH_EN_REG_OFFSET);
  }
  for (int i = 0; i < DMAC_NUMBER_OF_CHANNELS; i = i + 1) {
    // READDATA64[i]==1'b0
    if (BIT_PICK(ch_en_reg, i) == 0) {
      // free_channel_index = i;
      // break;
      return i;
    }
  }
  // return free_channel_index;
  return FREE_CHANNEL_WRONG_LABEL;
}

/* [P.136]
 * DMAC_ChEnReg [15:8] -> CH_EN_WE  [7:0] -> CH_EN
 * 0: DW_axi_dmac Channel is disabled.
 * 1: DW_axi_dmac Channel is enabled.
 */
inline void DMAC_CHx_enable_channel(uint32_t free_channel_index) {
  uint32_t CH_EN_WE          = 1 << (free_channel_index + 8);  // [(7:NC):8]
  uint32_t CH_EN             = 1 << free_channel_index;        // [(NC-1):0]
  uint32_t DMAC_CHEnReg_15_0 = CH_EN_WE | CH_EN;
  WRITE_BURST_32(VENUS_DMAC_ADDR,
                 DMAC_CH_EN_REG_OFFSET,
                 DMAC_CHEnReg_15_0);
}

/* [P.164] CHx_LLP */
inline void DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index) {
  assert((uint32_t)head_lli % 64 == 0);
  uint32_t first_lli    = (uint32_t)head_lli;
  uint64_t LOC          = (uint64_t)first_lli & 0xffffffffffffffc0;
  uint64_t LMS          = ((uint64_t)0 << 0);
  uint64_t RESERVED_5_1 = ((uint64_t)0 << 1);
  uint64_t CHx_LLP =
      LOC |           // [LOC][63:6] Starting Address in memory of next LLI
      RESERVED_5_1 |  // [5:1] Reserved and read as zero
      LMS;            // [LMS][0] LLI master Select (does not exist in this design)
  WRITE_BURST_64(VENUS_DMAC_ADDR,
                 DMAC_CH_LLP_REG_OFFSET_CH(free_channel_index),
                 CHx_LLP);
}

/* venus/src/testbench/peripherals/dmac_task_bfm.svh */
inline void DMAC_config(void) {
  /* [P.134] DMAC_CfgReg */
  // uint64_t RESERVED_63_2 = ((uint64_t)0 << 2);
  // uint64_t INT_EN        = ((uint64_t)1 << 1);
  // uint64_t DMAC_EN       = ((uint64_t)1 << 0);
  // uint64_t CFG_REG =
  //     RESERVED_63_2 |  // Reserved and read as zero
  //     INT_EN |         // [INT_EN][1] Global Interrupt Enable bit
  //     DMAC_EN;         // [DMAC_EN][0] DW_axi_dmac Enable bit
  uint64_t CFG_REG = 0x0000000000000003;
  WRITE_BURST_64(VENUS_DMAC_ADDR,
                 DMAC_CFG_REG_OFFSET,
                 CFG_REG);

  /* [P.140] DMAC_CommonReg_IntStatus_EnableReg */
  WRITE_BURST_64(VENUS_DMAC_ADDR,
                 DMAC_COMMON_STATUS_ENABLE_REG_OFFSET,
                 ALL_1_RESET);

  /* [P.140] DMAC_CommonReg_IntSignal_EnableReg */
  WRITE_BURST_64(VENUS_DMAC_ADDR,
                 DMAC_COMMON_SIGNAL_ENABLE_REG_OFFSET,
                 ALL_1_RESET);

  for (int i = 0; i < DMAC_NUMBER_OF_CHANNELS; i++) {
    /* [P.181] CHx_INTSTATUS_ENABLEREG */
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_STATUS_ENABLE_REG_OFFSET_CH(i),
                   ALL_1_RESET);
    /* [P.191] CHx_INTSIGNAL_ENABLEREG */
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_SIGNAL_ENABLE_REG_OFFSET_CH(i),
                   ALL_1_RESET);
  }
}

inline void DMAC_reset(void) {
  /* [P.144] DMAC_ResetReg */
  uint64_t DMAC_RST = ((uint64_t)1 << 0);  // [DMAC_RST][0] DMAC Reset Request bit
  WRITE_BURST_64(VENUS_DMAC_ADDR,
                 DMAC_RST_REG_REG_OFFSET,
                 DMAC_RST);
  uint64_t reset_reg = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_RST_REG_REG_OFFSET);
  while (BIT_PICK(reset_reg, 0) == 1) {
    reset_reg = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_RST_REG_REG_OFFSET);
  }
}

#define SHADOWREG_OR_LLI_VALID(N) ((uint64_t)N << 63)
#define SHADOWREG_OR_LLI_LAST(N)  ((uint64_t)N << 62)
#define RESERVED_61_59            ((uint64_t)0 << 59)
#define IOC_BlkTfr(N)             ((uint64_t)N << 58)
#define DST_STAT_EN(N)            ((uint64_t)N << 57)
#define SRC_STAT_EN(N)            ((uint64_t)N << 56)
#define AWLEN(N)                  ((uint64_t)(N) << 48)
#define AWLEN_EN(N)               ((uint64_t)N << 47)
#define ARLEN(N)                  ((uint64_t)(N) << 39)
#define ARLEN_EN(N)               ((uint64_t)N << 38)
#define AW_PROT(N)                ((uint64_t)N << 35)
#define AR_PROT(N)                ((uint64_t)N << 32)
#define RESERVED_31               ((uint64_t)0 << 31)
#define NonPosted_LastWrite_En(N) ((uint64_t)N << 30)
#define AW_CACHE(N)               ((uint64_t)N << 26)
#define AR_CACHE(N)               ((uint64_t)N << 22)
#define DST_MSIZE(N)              ((uint64_t)N << 18)
#define SRC_MSIZE(N)              ((uint64_t)N << 14)
#define DST_TR_WIDTH(N)           ((uint64_t)N << 11)
#define SRC_TR_WIDTH(N)           ((uint64_t)N << 8)
#define RESERVED_7                ((uint64_t)0 << 7)
#define DINC(N)                   ((uint64_t)N << 6)
#define RESERVED_5                ((uint64_t)0 << 5)
#define SINC(N)                   ((uint64_t)N << 4)
#define RESERVED_3                ((uint64_t)0 << 3)
#define DMS(N)                    ((uint64_t)N << 2)
#define RESERVED_1                ((uint64_t)0 << 1)
#define SMS(N)                    ((uint64_t)N << 0)

/* CHx_CTL: Control the DMA transfer [P.148] */
inline uint64_t CTL_config(uint64_t lli_last) {
  uint64_t CHx_CTL =
      SHADOWREG_OR_LLI_VALID(1) |                 // [SHADOWREG_OR_LLI_VALID][63] Shadow Register content/Linked List Item Valid
      SHADOWREG_OR_LLI_LAST(lli_last) |           // [SHADOWREG_OR_LLI_LAST][62] Last Shadow Register/Linked List Item. 0 = Not last Shadow Register/LLI 1 = Last Shadow Register/LLI
      RESERVED_61_59 |                            // [61:59] Reserved and read as zero
      IOC_BlkTfr(0) |                             // [IOC_BlkTfr][58] Interrupt On completion of Block Transfer
      DST_STAT_EN(0) |                            // [DST_STAT_EN][57] Destination Status Enable, Enable the logic to fetch status from destination peripheral of channel x pointed to by the content of CHx_DSTATAR register and stores it in CHx_DSTAT register.
      SRC_STAT_EN(0) |                            // [SRC_STAT_EN][56] Source Status Enable, Enable the logic to fetch status from source peripheral of channel x pointed to by the content of CH1_SSTATAR register and stores it in CHx_SSTAT register.
      AWLEN(destination_max_burst_length - 1) |   // [AWLEN][55:48] Destination Max Burst Length
      AWLEN_EN(1) |                               // [AWLEN_EN][47] Destination Burst Length Enable
      ARLEN(source_max_burst_length - 1) |        // [ARLEN][46:39] Source Max Burst Length
      ARLEN_EN(1) |                               // [ARLEN_EN][38] Source Burst Length Enable
      AW_PROT(0) |                                // [AW_PROT][37:35] AXI 'aw_prot' signal
      AR_PROT(0) |                                // [AR_PROT][34:32] AXI 'ar_prot' signal
      RESERVED_31 |                               // [31]Reserved and read as zero
      NonPosted_LastWrite_En(1) |                 // [NonPosted_LastWrite_En][30] Non Posted Last Write Enable, This bit decides whether posted writes can be used throughout the block transfer or not.
      AW_CACHE(0) |                               // [AW_CACHE][29:26] AXI 'aw_cache' signal
      AR_CACHE(0) |                               // [AR_CACHE][25:22] AXI 'ar_cache' signal
      DST_MSIZE(2) |                              // [DST_MSIZE][21:18] Destination Burst Transaction Length.  0010=8bytes?
      SRC_MSIZE(2) |                              // [SRC_MSIZE][17:14] Source Burst Transaction Length.  0010=8bytes?
      DST_TR_WIDTH(destination_transfer_width) |  // [DST_TR_WIDTH][13:11] Destination Transfer Width  011=64bits
      SRC_TR_WIDTH(source_transfer_width) |       // [SRC_TR_WIDTH][10:8] Source Transfer Width  011=64bits
      RESERVED_7 |                                // [7] Reserved and read as zero
      DINC(0) |                                   // [DINC][6] Destination Address Increment,0 = Increment 1 = No Change
      RESERVED_5 |                                // [5] Reserved and read as zero
      SINC(0) |                                   // [SINC][4] Source Address Increment,0 = Increment 1 = No Change
      RESERVED_3 |                                // [3] Reserved and read as zero
      DMS(0) |                                    // [DMS][2] Destination Master Select, 0 = AXI master 1 1 = AXI Master 2
      RESERVED_1 |                                // [1] Reserved and read as zero
      SMS(0);                                     // [SMS][0] Source Master Select, 0 = AXI master 1 1 = AXI Master 2
  return CHx_CTL;
}

#define RESERVED_63         ((uint64_t)0 << 63)
#define DST_OSR_LMT(N)      ((uint64_t)N << 59)
#define SRC_OSR_LMT(N)      ((uint64_t)N << 55)
#define LOCK_CH_L(N)        ((uint64_t)N << 53)
#define LOCK_CH(N)          ((uint64_t)N << 52)
#define CH_PRIOR(N)         ((uint64_t)N << 49)
#define RESERVED_48         ((uint64_t)0 << 48)
#define DST_PER(N)          ((uint64_t)N << 44)
#define RESERVED_43         ((uint64_t)0 << 43)
#define SRC_PER(N)          ((uint64_t)N << 39)
#define DST_HWHS_POL(N)     ((uint64_t)N << 38)
#define SRC_HWHS_POL(N)     ((uint64_t)N << 37)
#define HS_SEL_DST(N)       ((uint64_t)N << 36)
#define HS_SEL_SRC(N)       ((uint64_t)N << 35)
#define TT_FC(N)            ((uint64_t)N << 32)
#define RESERVED_31_4       ((uint64_t)0 << 4)
#define DST_MULTBLK_TYPE(N) ((uint64_t)N << 2)
#define SRC_MULTBLK_TYPE(N) ((uint64_t)N << 0)

/* [P.160]
 * Software programs the CHx_CFG register with appropriate values for the DMA transfer.
 * The SRC_MLTBLK_TYPE and/or DST_MLTBLK_TYPE bits must be set to 2'b11
 */
inline void CFG_config(uint32_t free_channel_index) {
  uint64_t CHx_CFG =
      RESERVED_63 |             // [63] Reserved and read as zero
      DST_OSR_LMT(0) |          // [DST_OSR_LMT][62:59] Destination Outstanding Request Limit
      SRC_OSR_LMT(0) |          // [SRC_OSR_LMT][58:55] Source Outstanding Request Limit
      LOCK_CH_L(0) |            // [LOCK_CH_L][54:53] Channel Lock Level
      LOCK_CH(0) |              // [LOCK_CH][52] Channel Lock bit
      CH_PRIOR(0) |             // [CH_PRIOR][51:49] Channel Priority
      RESERVED_48 |             // [48] Reserved and read as zero
      DST_PER(0) |              // [DST_PER][b:44] Assigns a hardware handshaking interface (0 - DMAX_NUM_HS_IF-1) to the destination of channel1 if the CHx_CFG.HS_SEL_DST field is 0; otherwise, this field is ignored.
      RESERVED_43 |             // [43] Reserved and read as zero
      SRC_PER(0) |              // [SRC_PER][b:39] Assigns a hardware handshaking interface (0 - DMAX_NUM_HS_IF-1) to the source of channel1 if the CHx_CFG.HS_SEL_SRC field is 0; otherwise, this field is ignored.
      DST_HWHS_POL(0) |         // [DST_HWHS_POL][38] Destination Hardware Handshaking Interface Polarity
      SRC_HWHS_POL(0) |         // [SRC_HWHS_POL][37] Source Hardware Handshaking Interface Polarity.
      HS_SEL_DST(1) |           // [HS_SEL_DST][36] Destination Software or Hardware Handshaking Select
      HS_SEL_SRC(1) |           // [HS_SEL_SRC][35] Source Software or Hardware Handshaking Select.
      TT_FC(0) |                // [TT_FC][34:32] Transfer Type and Flow Control (P161) 000 = Memory to Memory
      RESERVED_31_4 |           // [31:4] Reserved and read as zero
      DST_MULTBLK_TYPE(0b11) |  // [DST_MULTBLK_TYPE][3:2] Destination Multi Block Transfer Type. 11 = Linked List
      SRC_MULTBLK_TYPE(0b11);   // [SRC_MULTBLK_TYPE][1:0] Source Multi Block Transfer Type. 11 = Linked List
  WRITE_BURST_64(VENUS_DMAC_ADDR,
                 DMAC_CH_CFG_REG_OFFSET_CH(free_channel_index),
                 CHx_CFG);
}


#endif /* __DMA_H_ */
