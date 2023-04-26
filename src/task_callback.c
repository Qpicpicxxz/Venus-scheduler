#include "hw/blockcsr.h"
#include "task.h"

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

  // TODO: Decide what's in NumRetReg (this verison suppose just one batch of results)
  // TODO: For now, VenusBlock_RetAddr and VenusBlock_RetLen is just an offset, should be modified later
  for (int i = 0; actor->out[i][0] != NULL; i++) {
#ifndef SIMULATE_QEMU
    uint32_t RetAddr = read_burst(VenusBlock_CSR, VenusBlock_RetAddr(i));
    uint32_t RetLen  = read_busrt(VenusBlock_CSR, VenusBlock_RetLen(i));
#else
    uint32_t RetAddr = VenusBlock_RetAddr(i);
    uint32_t RetLen  = VenusBlock_RetLen(i);
#endif

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
    token->attr = SCALAR_LABEL;

    // if its the last data packet
    if (actor->out[i + 1][0] == NULL)
      // TODO: Under the pratical circumstance, should be
      // RetAddr_i = read_burst(VenusBlock_CSR, VenusBlock_RetAddr(i));
      // RetLen_i = read_busrt(VenusBlock_CSR, VenusBlock_RetLen(i));
      dma_transfer_link(alloc_addr, RetAddr, RetLen, cur_block, token);
    else
      dma_transfer_link(alloc_addr, RetAddr, RetLen, pseudo_block, token);
  }
}

void block_handler(block_t* n_block) {
  // check if this block needs to recycle result
  if ((n_block->flags & BLOCK_INFLIGHT) != 0) {
#ifdef DEBUG_SCHEDULER
    printf(YELLOW("BLOCK %d: Job done\n"), ((uint32_t)n_block - block_start) / sizeof(block_t) + 1);
#endif
    cur_block = n_block;
    _set_block_flag(n_block, BLOCK_RESULT);
    alloc_result();
  }
  // add current block into idle queue (if this block isn't in idle-fifo)
  if ((n_block->flags & BLOCK_INFIFO) == 0) {
    // mark this block has been put into idle-fifo
    _set_block_flag(n_block, BLOCK_INFIFO);
    put_queue(&block_q, (uint32_t)n_block);
  }
}

