#include "hw/addressmap.h"
#include "hw/clustercsr.h"
#include "task.h"
#include "ulib.h"

#define SIM_RESULT_LEN 4
/* dma.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, cluster_t* cluster, token_t* token);

/* internal variables */
static cluster_t* cur_cluster;

static inline void alloc_result(void) {
  uint32_t alloc_addr;
  // data_t* data;
  // token_t* token;
  uint32_t block_ret_num = READ_BURST_32(cur_cluster->base_addr, CLUSTER_CTRLREGS_OFFSET + VENUSCLUSTER_NUMRETREG_OFFSET);
  for (uint32_t i = 0; i < block_ret_num; i++) {
    uint32_t RetAddr = READ_BURST_32(cur_cluster->base_addr, CLUSTER_CTRLREGS_OFFSET + VENUSCLUSTER_RETADDRREG_OFFSET(i));
    uint32_t RetLen  = READ_BURST_32(cur_cluster->base_addr, CLUSTER_CTRLREGS_OFFSET + VENUSCLUSTER_RETLENREG_OFFSET(i));
    // TODO: handle all the return data
    alloc_addr = (uint32_t)malloc(RetLen);
    if (i == block_ret_num - 1)
      dma_transfer_link(alloc_addr, RetAddr, RetLen, cur_cluster, NULL);
    else
      dma_transfer_link(alloc_addr, RetAddr, RetLen, NULL, NULL);
  }
}

void cluster_handler(cluster_t* n_cluster) {
  cur_cluster = n_cluster;
  alloc_result();
  _set_cluster_flag(n_cluster, CLUSTER_DMA_UNLOAD);
  // TODO: handler cluster's interrupt...
}

