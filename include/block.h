#ifndef __BLOCK_H__
#define __BLOCK_H__

/* Mark the block as in-flight status,
 * 	to decide whether to recycle result or not */
#define BLOCK_INFLIGHT (uint32_t)(1 << 0)
/* Mark the block has already in idle fifo,
 *	to prevent block repeatedly pull up interrupt,
 *	which would cause fifo explosion */
#define BLOCK_INFIFO (uint32_t)(1 << 1)

#include "actor.h"

/*
 * We use a Block struct to bind venus block with task
 * -flags: some blocks status
 * -spm_addr: stratchpad memory mapped address
 * -actor: the current actor's pointer
 */
typedef struct Block {
  uint8_t flags;
  uint32_t spm_addr;
  actor_t *actor;
  uint32_t result; // JUST simulation!!!
} block_f;

typedef struct linger {
  block_f *block;
  data_t *data;
} linger_t;

static inline void _set_block_flag(struct Block *block, uint8_t flags) {
  block->flags |= flags;
}

static inline void _clear_block_flag(struct Block *block) { block->flags = 0; }

#endif /* __BLOCK_H__ */
