#include "block.h"
#include "hw/addressmap.h"
#include "hw/config.h"
#include "hw/devctrl.h"
#include "hw/dma.h"
#include "msg.h"
#include "types.h"
#include "ulib.h"

msg_t* msg_array[DMAC_NUMBER_OF_CHANNELS];
static msg_t msg_0;
static msg_t msg_1;
static msg_t msg_2;
static msg_t msg_3;
static msg_t msg_4;
static msg_t msg_5;
static msg_t msg_6;
static msg_t msg_7;

void msg_array_init(void) {
  msg_array[0] = &msg_0;
  msg_array[1] = &msg_1;
  msg_array[2] = &msg_2;
  msg_array[3] = &msg_3;
  msg_array[4] = &msg_4;
  msg_array[5] = &msg_5;
  msg_array[6] = &msg_6;
  msg_array[7] = &msg_7;
}

block_t block_stru[MAX_NUM_CLUSTERS][MAX_NUM_BLOCKS];

void block_struct_init() {
  for (int i = 0; i < MAX_NUM_CLUSTERS; i++) {
    for (int j = 0; j < MAX_NUM_BLOCKS; j++) {
      // initialize all the block's base address into block descriptor
      block_stru[i][j].base_addr = VENUS_CLUSTER_ADDR + CLUSTER_OFFSET(i) + BLOCK_OFFSET(j);
      // initialize all block flag into zero
      block_stru[i][j].flags = 0;
      block_stru[i][j].actor = NULL;
    }
  }
}

/*
 * 解开venus dma、 部分或者全部的venus block的硬复位
 * 硬复位：可以向Venus Block中的I-SPM和D-SPM写数据
 * 软复位：Block中的Pico处理器开始运行程序
 */

void devctrl_init(void) {
  // 使能AHB和DMA
  WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_AHB_DEV_RST_OFFSET, VENUS_AHB_DMA_EN);
  // 使能DEBUG UART
  WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_APB_DEV_RST_OFFSET, VENUS_DEBUG_UART_EN);
  // Test 2023-4-18: 测试，开启0_00 0_01 1_00 1_01 block
  WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(0), 0x80000003);
  WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(1), 0x80000003);
  // 解开全部的CLUSTER 和 AXI
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(0), CLUSTER_AXI_BLOCK_ALL_EN);
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(1), CLUSTER_AXI_BLOCK_ALL_EN);
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(2), CLUSTER_AXI_BLOCK_ALL_EN);
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(3), CLUSTER_AXI_BLOCK_ALL_EN);
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(4), CLUSTER_AXI_BLOCK_ALL_EN);
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(5), CLUSTER_AXI_BLOCK_ALL_EN);
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(6), CLUSTER_AXI_BLOCK_ALL_EN);
  // WRITE_BURST_32(VENUS_DEVCTRL_ADDR, VENUS_CLUSTER_DEV_RST_OFFSET_CLUSTER(7), CLUSTER_AXI_BLOCK_ALL_EN);
}


