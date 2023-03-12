#ifndef __BLOCK_H__
#define __BLOCK_H__

#define BLOCK_INFLIGHT (uint8_t)(1 << 0)
#define BLOCK_IDLE (uint8_t)(1 << 1)

#include "saddr.h"
#include "actor.h"

/*
 * We use a Block struct to bind venus block with task
 * -flags: some blocks status
 * -spm_addr: stratchpad memory mapped address
 * -actor: the current actor's pointer
 */
typedef struct Block {
	uint8_t   flags;
	uint32_t  task_addr;
	uint32_t  spm_addr;
	actor_t   *actor;
	uint32_t  result;	// just simulation!!!
} block_f;

typedef struct linger {
	block_f *block;
	data_t  *data;
} linger_t;

static inline void _set_block_flag(struct Block *block, uint8_t flags)
/* a |= b ---> a = a | b */
 { block->flags |= flags;
}

static inline void _clear_block_flag(struct Block *block)
{ block->flags = 0;}

#endif /* __BLOCK_H__ */
