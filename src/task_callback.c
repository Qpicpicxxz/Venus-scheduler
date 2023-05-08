#include "hw/addressmap.h"
#include "hw/blockcsr.h"
#include "task.h"
#include "ulib.h"

#define SIM_RESULT_LEN 4
/* dma.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, token_t* token);

/* internal variables */
static block_t* cur_block;

static inline void alloc_result(void) {
  actor_t* actor = cur_block->actor;
  uint32_t alloc_addr;
  block_t* pseudo_block = NULL;
  data_t* data;
  token_t* token;

  // 1. tranverse every return value
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    uint32_t RetAddr = READ_BURST_32(cur_block->base_addr, BLOCK_CTRLREGS_OFFSET + VENUSBLOCK_RETADDRREG_OFFSET(i));
    uint32_t RetLen  = READ_BURST_32(cur_block->base_addr, BLOCK_CTRLREGS_OFFSET + VENUSBLOCK_RETLENREG_OFFSET(i));

    alloc_addr = (uint32_t)malloc(RetLen);
    data       = (data_t*)malloc(sizeof(data));
    data->ptr  = alloc_addr;
    // initialize data lifecycle
    data->cnt = 0;
    // count data itself lifecycle
    for (int j = 0; actor->out[i][j] != NULL; j++)
      data->cnt++;

    token       = (token_t*)malloc(sizeof(token));
    token->data = data;
    // add a judge logic to distinguish if its a scalar(packet) or vector
    // we may can judge the retLen to distinguish whether its a scalar or vector
    // TODO: if it's a scalar, then mark as a scalar data
    // TODO: if it's a vector, then record the result vector's length
    token->attr = RetLen;

    for (int j = 0; actor->out[i][j] != NULL; j++)
      data->cnt++;
    // if its the last data packet
    if (actor->out[i + 1][0] == NULL)
      dma_transfer_link(alloc_addr, RetAddr, RetLen, cur_block, token);
    else
      dma_transfer_link(alloc_addr, RetAddr, RetLen, pseudo_block, token);
  }
}

void block_handler(block_t* n_block) {
  cur_block = n_block;
  _set_block_flag(n_block, BLOCK_RESULT);
  alloc_result();
}

