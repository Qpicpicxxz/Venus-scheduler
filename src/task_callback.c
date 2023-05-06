#include "hw/blockcsr.h"
#include "task.h"

#define SIM_RESULT_LEN 4
/* dma.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, token_t* token);

/* internal variables */
static block_t* cur_block;

// SIMULATE ONLY: dynamic dependencies
#define RATEMATCH_NUM 4

static inline void alloc_result(void) {
  actor_t* actor = cur_block->actor;
  uint32_t alloc_addr;
  block_t* pseudo_block = NULL;
  data_t* data;
  token_t* token;
  uint32_t task_ret_num = 0;
  // in real sence, should be
  // READ_BURST_32(cur_block->base_addr, BLOCK_CTRLREGS_OFFSET + VENUSBLOCK_RETNUMREG_OFFSET);
  uint32_t block_ret_num = RATEMATCH_NUM;  // dynamic

  // TODO: Should I add an attribute into actor-descriptor to indicate res_num? To avoid traverse all fifo...

  // count current actor's task_ret_num (static)
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    task_ret_num++;
  }

  // three situations: multiple batches of result, normal situation and dynamic dependencies
  if (block_ret_num > task_ret_num) {
    // multiple batch of result
    uint32_t batch = block_ret_num / task_ret_num;

    // simulate only...
    batch = 1;

    for (int k = 0; k < batch; k++) {
      for (int i = 0; actor->out[i][0] != NULL; i++) {
        // malloc data + token descriptor
        // initialize data + token descriptor
        uint32_t RetAddr = VenusBlock_RetAddr(i);
        uint32_t RetLen  = VenusBlock_RetLen(i);
        alloc_addr       = (uint32_t)malloc(RetLen);
        data             = (data_t*)malloc(sizeof(data));
        data->ptr        = alloc_addr;
        // initialize data lifecycle
        data->cnt = 0;
        // count data itself lifecycle
        for (int j = 0; actor->out[i][j] != NULL; j++)
          data->cnt++;
        token       = (token_t*)malloc(sizeof(token));
        token->data = data;
        // add a judge logic to distinguish if its a scalar(packet) or vector
        token->attr = SCALAR_LABEL;

        // inform DMA
        // if its the last data packet
        if (actor->out[i + 1][0] == NULL)
          dma_transfer_link(alloc_addr, RetAddr, RetLen, cur_block, token);
        else
          dma_transfer_link(alloc_addr, RetAddr, RetLen, pseudo_block, token);
      }
    }
  } else if (block_ret_num == task_ret_num) {
    // normal situation
    for (int i = 0; actor->out[i][0] != NULL; i++) {
      // malloc data + token descriptor
      // initialize data + token descriptor
      uint32_t RetAddr = VenusBlock_RetAddr(i);
      uint32_t RetLen  = VenusBlock_RetLen(i);
      alloc_addr       = (uint32_t)malloc(RetLen);
      data             = (data_t*)malloc(sizeof(data));
      data->ptr        = alloc_addr;
      // initialize data lifecycle
      data->cnt = 0;
      // count data itself lifecycle
      for (int j = 0; actor->out[i][j] != NULL; j++)
        data->cnt++;
      token       = (token_t*)malloc(sizeof(token));
      token->data = data;
      // add a judge logic to distinguish if its a scalar(packet) or vector
      token->attr = SCALAR_LABEL;

      // inform DMA
      // if its the last data packet
      if (actor->out[i + 1][0] == NULL)
        dma_transfer_link(alloc_addr, RetAddr, RetLen, cur_block, token);
      else
        dma_transfer_link(alloc_addr, RetAddr, RetLen, pseudo_block, token);
    }
  } else {
    // dynamic dependencies
    for (int i = 0; i < block_ret_num; i++) {
      uint32_t RetAddr = VenusBlock_RetAddr(i);
      uint32_t RetLen  = VenusBlock_RetLen(i);
      alloc_addr       = (uint32_t)malloc(RetLen);
      data             = (data_t*)malloc(sizeof(data));
      data->ptr        = alloc_addr;
      // initialize data lifecycle
      data->cnt = 0;
      // count data itself lifecycle
      for (int j = 0; actor->out[i][j] != NULL; j++)
        data->cnt++;

      token       = (token_t*)malloc(sizeof(token));
      token->data = data;
      // add a judge logic to distinguish if its a scalar(packet) or vector
      token->attr = SCALAR_LABEL;

      // inform DMA
      // if its the last data packet
      if (i == block_ret_num - 1)
        dma_transfer_link(alloc_addr, RetAddr, RetLen, cur_block, token);
      else
        dma_transfer_link(alloc_addr, RetAddr, RetLen, pseudo_block, token);
    }
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

