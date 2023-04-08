#include "task.h"

/* dma.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, data_t* data);

/* internal variables */
static block_t* cur_block;

static inline void alloc_result(void) {
  // prpare return result's store space in advance
  uint32_t alloc_addr = (uint32_t)malloc(cur_block->actor->result_len);
  actor_t* actor      = cur_block->actor;

  data_t* data = malloc(sizeof(data_t));
  data->ptr    = alloc_addr;         // data pointer
  data->len    = actor->result_len;  // data length
  data->cnt    = actor->nxt_num;     // data lifecycle

  // inform DMA
  dma_transfer_link(alloc_addr, DATA1_ADDR, cur_block->actor->result_len, cur_block, data);
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

