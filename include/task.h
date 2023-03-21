#ifndef __TASK_H__
#define __TASK_H__

#include "types.h"
#include "fifo.h"
#include "block.h"
#include "actor.h"
#include "daddr.h"
#include "codeAddr.h"

#define NUM_BLOCKS 8

extern list_t *actor_l;
extern list_t *ready_l;
extern uint8_t actor_index;
extern uint32_t actor_space;
extern uint32_t actor_start;

/* actor create */
extern actor_t *actor_create(uint32_t taskStart, uint32_t taskLen, uint32_t result_len);
extern void edge_make(actor_t *dep, actor_t *suc);
extern void packet_input(actor_t *actor, uint32_t data_addr, uint32_t data_len);
extern void assign_root(actor_t *actor);
extern void assign_sink(actor_t *actor);

#endif /* __TASK_H__ */
