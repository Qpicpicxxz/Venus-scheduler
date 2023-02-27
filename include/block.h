#ifndef __BLOCK_H
#define __BLOCK_H

#define BLOCK_BUSY (uint8_t)(1 << 0)

/*
  We use a Block struct to bind venus block with task
  -flags: some blocks status
  -prev_actor_addr: previous actor's address
  -next_actor_addr: successor actor's address
*/
typedef struct Block {
	uint8_t flags;
	uint32_t task_addr;
	uint32_t prev_actor_addr;
	uint32_t next_actor_addr;
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

#endif
