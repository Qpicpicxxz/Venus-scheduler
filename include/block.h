#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "defs.h"

/* Mark the block as in-flight status,
 * 	to decide whether to recycle result or not */
#define BLOCK_INFLIGHT (uint8_t)(1 << 0)
/* Mark the block has already in idle fifo,
 *	to prevent block repeatedly pull up interrupt,
 *	which would cause fifo explosion */
#define BLOCK_INFIFO (uint8_t)(1 << 1)

/*
 * We use a Block struct to bind venus block with task
 * -flags: some blocks status
 * -spm_addr: stratchpad memory mapped address
 * -actor: the current actor's pointer
 */
typedef struct block {
  uint32_t flags;
  uint32_t spm_addr;
  actor_t* actor;
} block_t;

typedef struct linger {
  block_t* block;
  data_t* data;
} linger_t;

static inline void _set_block_flag(block_t* block, uint8_t flags) { block->flags |= flags; }

static inline void _clear_block_flag(block_t* block) { block->flags = 0; }

#endif /* __BLOCK_H__ */
