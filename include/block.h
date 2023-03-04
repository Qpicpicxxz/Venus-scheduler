#ifndef __BLOCK_H__
#define __BLOCK_H__

#define BLOCK_BUSY (uint8_t)(1 << 0)

#include "saddr.h"
#include "actor.h"

/*
 * We use a Block struct to bind venus block with task
 * -flags: some blocks status
 * -spm_addr: stratchpad memory mapped address
 * -prev_actor: previous actor's pointer
 * -next_actor: successor actor's pointer
 */
typedef struct Block {
	uint8_t   flags;
	uint32_t  task_addr;
	uint32_t  spm_addr;
	actor_t *prev_actor;
	actor_t *next_actor;
} block_f;

static inline int _is_block_done(struct Block *block) {
	if (block->flags & BLOCK_BUSY) {
		return 0;
	} else {
		return 1;
	}
}

static inline void _set_block_flag(struct Block *block, uint8_t m_flags) { block->flags |= m_flags;
}

static inline void _clear_block_flag(struct Block *block)
{ block->flags = 0;}

#endif /* __BLOCK_H__ */
