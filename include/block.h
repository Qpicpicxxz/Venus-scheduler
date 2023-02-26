#ifndef __BLOCK_H
#define __BLOCK_H

#define BLOCK_BUSY (uint8_t)(1 << 0)

typedef struct Block {
	uint8_t flags;
} block_f;

static inline int _is_block_idle(struct Block *block) {
	if (block->flags & BLOCK_BUSY) {
		return 0;
	} else {
		return 1;
	}
}

static inline void _set_block_flag(struct Block *block, uint8_t m_flags) { block->flags |= m_flags;
}

#endif
