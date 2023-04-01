#include "dma/cfg.h"
#include "dma/common.h"

/* phy_interface.c */
extern void DMAC_CHx_cfg_write(uint64_t CHx_CFG, uint32_t free_channel_index);

/* [P.160]
 * Software programs the CHx_CFG register with appropriate values for the DMA transfer.
 * The SRC_MLTBLK_TYPE and/or DST_MLTBLK_TYPE bits must be set to 2'b11
 */
void cfg_config(uint32_t free_channel_index) {
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
#ifdef SIMULATE_QEMU
  printf("CHx_CFG is %x%x\n", (uint32_t)(CHx_CFG >> 32), (uint32_t)(CHx_CFG));
#endif
  DMAC_CHx_cfg_write(CHx_CFG, free_channel_index);
}
