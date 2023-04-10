#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "defs.h"

/*
 * flags format:                             3              2              1             0
 * --+-----------------------------------------------------------------------------------+--
 *   | 0000 0000 0000 0000 0000 0000 0000 0  |      0       |      0       |      0      |
 * --+-----------------------------------------------------------------------------------+--
 *               Reserved                    | BLOCK_RESULT | BLOCK_INFIFO | BLOCK_INFLIGHT
 */

/* Mark the block as in-flight status,
 * 	to decide whether to recycle result or not */
#define BLOCK_INFLIGHT (uint32_t)(1 << 0)
/* Mark the block has already in idle fifo,
 *	to prevent block repeatedly pull up interrupt,
 *	which would cause fifo explosion */
#define BLOCK_INFIFO (uint32_t)(1 << 1)
/* Mark current block should recycle result from SPM to DDR,
 *      to control DMA callback handler do different action */
#define BLOCK_RESULT (uint32_t)(1 << 2)

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
  list_t* data_list;
} linger_t;

static inline void _set_block_flag(block_t* block, uint8_t flags) { block->flags |= flags; }

static inline void _clear_block_flag(block_t* block) { block->flags = 0; }

#endif /* __BLOCK_H__ */

