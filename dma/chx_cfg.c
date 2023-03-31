#include "dma/cfg.h"
#include "dma/common.h"

/* [P.160]
 * Software programs the CHx_CFG register with appropriate values for the DMA transfer.
 * The SRC_MLTBLK_TYPE and/or DST_MLTBLK_TYPE bits must be set to 2'b11
 */
uint64_t cfg_config(void) {
  uint64_t CHx_CFG =
      RESERVED_63 |             // Reserved and read as zero [63]
      DST_OSR_LMT(0) |          // [DST_OSR_LMT] Destination Outstanding Request Limit
      SRC_OSR_LMT(0) |          // [SRC_OSR_LMT] Source Outstanding Request Limit
      LOCK_CH_L(0) |            // [LOCK_CH_L] Channel Lock Level
      LOCK_CH(0) |              // [LOCK_CH] Channel Lock bit
      CH_PRIOR(0) |             // [CH_PRIOR] Channel Priority
      RESERVED_48 |             // Reserved and read as zero [48]
      DST_PER(0) |              // [DST_PER] Assigns a hardware handshaking interface (0 - DMAX_NUM_HS_IF-1) to the destination of channel1 if the CHx_CFG.HS_SEL_DST field is 0; otherwise, this field is ignored.
      RESERVED_43 |             // Reserved and read as zero [43]
      SRC_PER(0) |              // [SRC_PER] Assigns a hardware handshaking interface (0 - DMAX_NUM_HS_IF-1) to the source of channel1 if the CHx_CFG.HS_SEL_SRC field is 0; otherwise, this field is ignored.
      DST_HWHS_POL(0) |         // [DST_HWHS_POL] Destination Hardware Handshaking Interface Polarity
      SRC_HWHS_POL(0) |         // [SRC_HWHS_POL] Source Hardware Handshaking Interface Polarity.
      HS_SEL_DST(1) |           // [HS_SEL_DST] Destination Software or Hardware Handshaking Select
      HS_SEL_SRC(1) |           // [HS_SEL_SRC] Source Software or Hardware Handshaking Select.
      TT_FC(0) |                // [TT_FC] Transfer Type and Flow Control (P161) 000 = Memory to Memory
      RESERVED_31_4 |           // Reserved and read as zero [31:4]
      DST_MULTBLK_TYPE(0b11) |  // [DST_MULTBLK_TYPE] Destination Multi Block Transfer Type. 11 = Linked List
      SRC_MULTBLK_TYPE(0b11);   // [SRC_MULTBLK_TYPE] Source Multi Block Transfer Type. 11 = Linked List
  return CHx_CFG;
}
