#include "dma.h"

extern void dma_transmit_done_callback(uint32_t channel_index);

void DMAC_CommonReg_interrupt_handler(void) {
  /* [P.141] DMAC_CommonReg_IntStatusReg */
  uint64_t DMAC_CommonReg_IntStatusReg         = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_COMMON_INTR_STATUS_REG_OFFSET);
  uint8_t SLVIF_UndefinedReg_DEC_ERR_IntStat   = BIT_PICK(DMAC_CommonReg_IntStatusReg, 8);
  uint8_t SLVIF_CommonReg_WrOnHold_ERR_IntStat = BIT_PICK(DMAC_CommonReg_IntStatusReg, 3);
  uint8_t SLVIF_CommonReg_RD2WO_ERR_IntStat    = BIT_PICK(DMAC_CommonReg_IntStatusReg, 2);
  uint8_t SLVIF_CommonReg_WR2RO_ERR_IntStat    = BIT_PICK(DMAC_CommonReg_IntStatusReg, 1);
  uint8_t SLVIF_CommonReg_DEC_ERR_IntStat      = BIT_PICK(DMAC_CommonReg_IntStatusReg, 0);

  /* [P.138] DMAC_CommonReg_IntClearReg */
  if (SLVIF_UndefinedReg_DEC_ERR_IntStat) {
    printf("[Hardware] SCHEDULER: DMAC Slave Interface Undefined register Decode Error Interrupt occurred... $stop\n");
    uint64_t Clear_SLVIF_UndefinedReg_DEC_ERR_IntStat = ((uint64_t)1 << 8);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_COMMON_INTR_CLEAR_REG_OFFSET,
                   Clear_SLVIF_UndefinedReg_DEC_ERR_IntStat);
  } else if (SLVIF_CommonReg_WrOnHold_ERR_IntStat) {
    printf("[Hardware] SCHEDULER: DMAC Slave Interface Common Register Write On Hold Error Interrupt occurred... $stop\n");
    uint64_t Clear_SLVIF_CommonReg_WrOnHold_ERR_IntStat = ((uint64_t)1 << 3);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_COMMON_INTR_CLEAR_REG_OFFSET,
                   Clear_SLVIF_CommonReg_WrOnHold_ERR_IntStat);
  } else if (SLVIF_CommonReg_RD2WO_ERR_IntStat) {
    printf("[Hardware] SCHEDULER: DMAC Slave Interface Common Register Read to Write only Error Interrupt occurred... $stop\n");
    uint64_t Clear_SLVIF_CommonReg_RD2WO_ERR_IntStat = ((uint64_t)1 << 2);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_COMMON_INTR_CLEAR_REG_OFFSET,
                   Clear_SLVIF_CommonReg_RD2WO_ERR_IntStat);
  } else if (SLVIF_CommonReg_WR2RO_ERR_IntStat) {
    printf("[Hardware] SCHEDULER: DMAC Slave Interface Common Register Write to Read Only Error Interrupt occurred... $stop\n");
    uint64_t Clear_SLVIF_CommonReg_WR2RO_ERR_IntStat = ((uint64_t)1 << 1);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_COMMON_INTR_CLEAR_REG_OFFSET,
                   Clear_SLVIF_CommonReg_WR2RO_ERR_IntStat);
  } else if (SLVIF_CommonReg_DEC_ERR_IntStat) {
    printf("[Hardware] SCHEDULER: DMAC Slave Slave Interface Common Register Decode Error Interrupt occurred... $stop\n");
    uint64_t Clear_SLVIF_CommonReg_DEC_ERR_IntStat = ((uint64_t)1 << 0);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_COMMON_INTR_CLEAR_REG_OFFSET,
                   Clear_SLVIF_CommonReg_DEC_ERR_IntStat);
  } else {
    printf("[Hardware] SCHEDULER: Fatal Error, An Unexpected DMAC interrupt occurred, please check... $stop\n");
  }
}

void DMAC_CHx_interrupt_handler(uint32_t channel_index) {
  /* [P.186] CHx_INTSTATUSREG */
  // printf("DMAC Channel %d Interrupt occurred...\n", channel_index);
  /* CHx_INTSTATUSEREG[63:32] reserved as zero */
  uint32_t CHx_INTSTATUSREG                      = READ_BURST_32(VENUS_DMAC_ADDR, DMAC_CH_INTR_STATUS_REG_OFFSET_CH(channel_index));
  uint8_t CH_ABORTED_IntStat                     = BIT_PICK(CHx_INTSTATUSREG, 31);
  uint8_t CH_DISABLED_IntStat                    = BIT_PICK(CHx_INTSTATUSREG, 30);
  uint8_t CH_SUSPENDED_IntStat                   = BIT_PICK(CHx_INTSTATUSREG, 29);
  uint8_t CH_SRC_SUSPENDED_IntStat               = BIT_PICK(CHx_INTSTATUSREG, 28);
  uint8_t ChLock_Cleared_IntStat                 = BIT_PICK(CHx_INTSTATUSREG, 27);
  uint8_t SLVIF_WrOnHold_ERR_IntStat             = BIT_PICK(CHx_INTSTATUSREG, 21);
  uint8_t SLVIF_ShadowReg_WrOn_Valid_ERR_IntStat = BIT_PICK(CHx_INTSTATUSREG, 20);
  uint8_t SLVIF_WrOnChEn_ERR_IntStat             = BIT_PICK(CHx_INTSTATUSREG, 19);
  uint8_t SLVIF_RD2WO_ERR_IntStat                = BIT_PICK(CHx_INTSTATUSREG, 18);
  uint8_t SLVIF_WR2RO_ERR_IntStat                = BIT_PICK(CHx_INTSTATUSREG, 17);
  uint8_t SLVIF_DEC_ERR_IntStat                  = BIT_PICK(CHx_INTSTATUSREG, 16);
  uint8_t SLVIF_MultiBlkType_ERR_IntStat         = BIT_PICK(CHx_INTSTATUSREG, 14);
  uint8_t ShadowReg_Or_LLI_Invalid_ERR_IntStat   = BIT_PICK(CHx_INTSTATUSREG, 13);
  uint8_t LLI_WR_SLV_ERR_IntStat                 = BIT_PICK(CHx_INTSTATUSREG, 12);
  uint8_t LLI_RD_SLV_ERR_IntStat                 = BIT_PICK(CHx_INTSTATUSREG, 11);
  uint8_t LLI_WR_DEC_ERR_IntStat                 = BIT_PICK(CHx_INTSTATUSREG, 10);
  uint8_t LLI_RD_DEC_ERR_IntStat                 = BIT_PICK(CHx_INTSTATUSREG, 9);
  uint8_t DST_SLV_ERR_IntStat                    = BIT_PICK(CHx_INTSTATUSREG, 8);
  uint8_t SRC_SLV_ERR_IntStat                    = BIT_PICK(CHx_INTSTATUSREG, 7);
  uint8_t DST_DEC_ERR_IntStat                    = BIT_PICK(CHx_INTSTATUSREG, 6);
  uint8_t SRC_DEC_ERR_IntStat                    = BIT_PICK(CHx_INTSTATUSREG, 5);
  uint8_t DST_TransComp_IntStat                  = BIT_PICK(CHx_INTSTATUSREG, 4);
  uint8_t SRC_TransComp_IntStat                  = BIT_PICK(CHx_INTSTATUSREG, 3);
  uint8_t DMA_TFR_DONE_IntStat                   = BIT_PICK(CHx_INTSTATUSREG, 1);
  uint8_t BLOCK_TFR_DONE_IntStat                 = BIT_PICK(CHx_INTSTATUSREG, 0);

  /* [P.195] CHx_INTCLEARREG */
  if (CH_ABORTED_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Channel Aborted Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [31] $stop\n", channel_index);
    uint64_t Clear_CH_ABORTED_IntStat = ((uint64_t)1 << 31);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_CH_ABORTED_IntStat);
  } else if (CH_DISABLED_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Channel Disabled Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [30] $stop\n", channel_index);
    uint64_t Clear_CH_DISABLED_IntStat = ((uint64_t)1 << 30);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_CH_DISABLED_IntStat);
  } else if (CH_SUSPENDED_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Channel Suspended Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [29] $stop\n", channel_index);
    uint64_t Clear_CH_SUSPENDED_IntStat = ((uint64_t)1 << 29);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_CH_SUSPENDED_IntStat);
  } else if (CH_SRC_SUSPENDED_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Channel Source Suspended Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [28] $stop\n", channel_index);
    uint64_t Clear_CH_SRC_SUSPENDED_IntStat = ((uint64_t)1 << 28);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_CH_SRC_SUSPENDED_IntStat);
  } else if (ChLock_Cleared_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Channel Lock Cleared Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [27] $stop\n", channel_index);
    uint64_t Clear_ChLock_Cleared_IntStat = ((uint64_t)1 << 27);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_ChLock_Cleared_IntStat);
  } else if (SLVIF_WrOnHold_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Slave Interface Write On Hold Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [21] $stop\n", channel_index);
    uint64_t Clear_SLVIF_WRONHOLD_ERR_IntStat = ((uint64_t)1 << 21);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SLVIF_WRONHOLD_ERR_IntStat);
  } else if (SLVIF_ShadowReg_WrOn_Valid_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Slave Interface Shadow Register Write On Valid Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [20] $stop\n", channel_index);
    uint64_t Clear_SLVIF_SHADOWREG_WRON_VALID_ERR_IntStat = ((uint64_t)1 << 20);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SLVIF_SHADOWREG_WRON_VALID_ERR_IntStat);
  } else if (SLVIF_WrOnChEn_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Slave Interface Write On Channel Enabled Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [19] $stop\n", channel_index);
    uint64_t Clear_SLVIF_WRONCHEN_ERR_IntStat = ((uint64_t)1 << 19);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SLVIF_WRONCHEN_ERR_IntStat);
  } else if (SLVIF_RD2WO_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Slave Interface Read to Write Only Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [18] $stop\n", channel_index);
    uint64_t Clear_SLVIF_RD2WO_ERR_IntStat = ((uint64_t)1 << 18);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SLVIF_RD2WO_ERR_IntStat);
  } else if (SLVIF_WR2RO_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Slave Interface Write to Read Only Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [17] $stop\n", channel_index);
    uint64_t Clear_SLVIF_WR2RO_ERR_IntStat = ((uint64_t)1 << 17);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SLVIF_WR2RO_ERR_IntStat);
  } else if (SLVIF_DEC_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Slave Interface Decode Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [16] $stop\n", channel_index);
    uint64_t Clear_SLVIF_DEC_ERR_IntStat = ((uint64_t)1 << 16);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SLVIF_DEC_ERR_IntStat);
  } else if (SLVIF_MultiBlkType_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Slave Interface Multi Block Type Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [14] $stop\n", channel_index);
    uint64_t Clear_SLVIF_MULTIBLKTYPE_ERR_IntStat = ((uint64_t)1 << 14);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SLVIF_MULTIBLKTYPE_ERR_IntStat);
  } else if (ShadowReg_Or_LLI_Invalid_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Shadow Register or LLI Invalid Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [13] $stop\n", channel_index);
    uint64_t Clear_SHADOWREG_OR_LLI_INVALID_ERR_IntStat = ((uint64_t)1 << 13);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SHADOWREG_OR_LLI_INVALID_ERR_IntStat);
  } else if (LLI_WR_SLV_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d LLI Write Slave Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [12] $stop\n", channel_index);
    uint64_t Clear_LLI_WR_SLV_ERR_IntStat = ((uint64_t)1 << 12);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_LLI_WR_SLV_ERR_IntStat);
  } else if (LLI_RD_SLV_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d LLI Read Slave Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [11] $stop\n", channel_index);
    uint64_t Clear_LLI_RD_SLV_ERR_IntStat = ((uint64_t)1 << 11);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_LLI_RD_SLV_ERR_IntStat);
  } else if (LLI_WR_DEC_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d LLI Write Decode Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [10] $stop\n", channel_index);
    uint64_t Clear_LLI_WR_DEC_ERR_IntStat = ((uint64_t)1 << 10);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_LLI_WR_DEC_ERR_IntStat);
  } else if (LLI_RD_DEC_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d LLI Read Decode Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [9] $stop\n", channel_index);
    uint64_t Clear_LLI_RD_DEC_ERR_IntStat = ((uint64_t)1 << 9);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_LLI_RD_DEC_ERR_IntStat);
  } else if (DST_SLV_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Destination Slave Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [8] $stop\n", channel_index);
    uint64_t Clear_DST_SLV_ERR_IntStat = ((uint64_t)1 << 8);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_DST_SLV_ERR_IntStat);
  } else if (SRC_SLV_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Source Slave Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [7] $stop\n", channel_index);
    uint64_t Clear_SRC_SLV_ERR_IntStat = ((uint64_t)1 << 7);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SRC_SLV_ERR_IntStat);
  } else if (DST_DEC_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Destination Decode Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [6] $stop\n", channel_index);
    uint64_t Clear_DST_DEC_ERR_IntStat = ((uint64_t)1 << 6);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_DST_DEC_ERR_IntStat);
  } else if (SRC_DEC_ERR_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Source Decode Error Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [5] $stop\n", channel_index);
    uint64_t Clear_SRC_DEC_ERR_IntStat = ((uint64_t)1 << 5);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SRC_DEC_ERR_IntStat);
  } else if (DST_TransComp_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Destination Transaction Completed Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [4] $stop\n", channel_index);
    uint64_t Clear_DST_TRANSCOMP_IntStat = ((uint64_t)1 << 4);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_DST_TRANSCOMP_IntStat);
  } else if (SRC_TransComp_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Source Transaction Completed Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [3] $stop\n", channel_index);
    uint64_t Clear_SRC_TRANSCOMP_IntStat = ((uint64_t)1 << 3);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_SRC_TRANSCOMP_IntStat);
  } else if (DMA_TFR_DONE_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d DMA Transfer Done Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d transfer done\n", channel_index);
    uint64_t Clear_DMA_TFR_DONE_IntStat = ((uint64_t)1 << 1);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_DMA_TFR_DONE_IntStat);
    // TODO: DMA done callback...
    dma_transmit_done_callback(channel_index);
  } else if (BLOCK_TFR_DONE_IntStat) {
    // printf("[Hardware] SCHEDULER: DMAC Channel %d Block Transfer Done Interrupt occurred... $stop\n", channel_index);
    printf("DMAC Channel %d IRQ [0] $stop\n", channel_index);
    uint64_t Clear_BLOCK_TFR_DONE = ((uint64_t)1 << 0);
    WRITE_BURST_64(VENUS_DMAC_ADDR,
                   DMAC_CH_INTR_CLEAR_REG_OFFSET_CH(channel_index),
                   Clear_BLOCK_TFR_DONE);
  } else {
    printf("DMAC Fatal Error $stop\n");
  }
}

void DMAC_interrupt_handler(void) {
  /* [P.137] DMAC_IntStatusReg */
  uint64_t DMAC_IntStatusReg = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_INTR_STATUS_REG_OFFSET);
  uint8_t CommonReg_IntStat  = BIT_PICK(DMAC_IntStatusReg, 16);
  uint8_t CH8_IntStat        = BIT_PICK(DMAC_IntStatusReg, 7);
  uint8_t CH7_IntStat        = BIT_PICK(DMAC_IntStatusReg, 6);
  uint8_t CH6_IntStat        = BIT_PICK(DMAC_IntStatusReg, 5);
  uint8_t CH5_IntStat        = BIT_PICK(DMAC_IntStatusReg, 4);
  uint8_t CH4_IntStat        = BIT_PICK(DMAC_IntStatusReg, 3);
  uint8_t CH3_IntStat        = BIT_PICK(DMAC_IntStatusReg, 2);
  uint8_t CH2_IntStat        = BIT_PICK(DMAC_IntStatusReg, 1);
  uint8_t CH1_IntStat        = BIT_PICK(DMAC_IntStatusReg, 0);
  if (CommonReg_IntStat)
    DMAC_CommonReg_interrupt_handler();
  else if (CH8_IntStat)
    DMAC_CHx_interrupt_handler(7);
  else if (CH7_IntStat)
    DMAC_CHx_interrupt_handler(6);
  else if (CH6_IntStat)
    DMAC_CHx_interrupt_handler(5);
  else if (CH5_IntStat)
    DMAC_CHx_interrupt_handler(4);
  else if (CH4_IntStat)
    DMAC_CHx_interrupt_handler(3);
  else if (CH3_IntStat)
    DMAC_CHx_interrupt_handler(2);
  else if (CH2_IntStat)
    DMAC_CHx_interrupt_handler(1);
  else if (CH1_IntStat)
    DMAC_CHx_interrupt_handler(0);
  else
    printf("[Hardware] SCHEDULER: Fatal Error, An Unexpected DMAC interrupt occurred, please check... $stop\n");
}

uint32_t DMAC_get_free_channel(void) {
  uint64_t ch_en_reg = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_CH_EN_REG_OFFSET);
  uint32_t free_channel_index;
  // while(READDATA64[(DMAC_NUMBER_OF_CHANNELS-1):0]=={(DMAC_NUMBER_OF_CHANNELS){1'b1}})
  while ((ch_en_reg & CHANNEL_MASK) == CHANNEL_MASK) {
    ch_en_reg = READ_BURST_64(VENUS_DMAC_ADDR, DMAC_CH_EN_REG_OFFSET);
  }
  for (int i = 0; i < DMAC_NUMBER_OF_CHANNELS; i = i + 1) {
    // READDATA64[i]==1'b0
    if (BIT_PICK(ch_en_reg, i) == 0) {
      free_channel_index = i;
      break;
    }
  }
  return free_channel_index;
}

/* [P.136]
 * DMAC_ChEnReg [15:8] -> CH_EN_WE  [7:0] -> CH_EN
 * 0: DW_axi_dmac Channel is disabled.
 * 1: DW_axi_dmac Channel is enabled.
 */
void DMAC_CHx_enable_channel(uint32_t free_channel_index) {
  uint32_t CH_EN_WE          = 1 << (free_channel_index + 8);  // [(7:NC):8]
  uint32_t CH_EN             = 1 << free_channel_index;        // [(NC-1):0]
  uint32_t DMAC_CHEnReg_15_0 = CH_EN_WE | CH_EN;
  WRITE_BURST_32(VENUS_DMAC_ADDR,
                 DMAC_CH_EN_REG_OFFSET,
                 DMAC_CHEnReg_15_0);
}

/* [P.164] CHx_LLP */
void DMAC_CHx_specify_first_lli(lli_t* head_lli, uint32_t free_channel_index) {
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

