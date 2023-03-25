#ifndef __TASK_H__
#define __TASK_H__

#define NUM_BLOCKS 8
#define MAXFIFO 8
#define MAXIO 4

#include "actor.h"
#include "block.h"
#include "codeAddr.h"
#include "common.h"
#include "daddr.h"
#include "fifo.h"
#include "linkedlist.h"

extern list_t* actor_l;
extern list_t* ready_l;
extern uint8_t actor_index;
extern uint32_t actor_space;
extern uint32_t actor_start;

/* actor create */
actor_t* actor_create(uint32_t taskStart, uint32_t taskLen, uint32_t result_len);
void edge_make(actor_t* src, actor_t* snk);
void packet_input(actor_t* actor, uint32_t data_addr, uint32_t data_len);
void assign_root(actor_t* actor);
void assign_sink(actor_t* actor);

#endif /* __TASK_H__ */
