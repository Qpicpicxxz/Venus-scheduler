#ifndef __TASK_H__
#define __TASK_H__

#define NUM_BLOCKS 8
#define NUM_PACKET 5
#define NUM_BUFFER 8
#define NUM_DEGREE 10

#define MAXFIFO (NUM_BUFFER + 1)
#define MAXIO (NUM_DEGREE + 1)

#include "actor.h"
#include "block.h"
#include "codeaddr.h"
#include "common.h"
#include "daddr.h"
#include "fifo.h"
#include "linkedlist.h"

extern list_t* actor_l;
extern list_t* ready_l;
extern queue_t block_q;
extern uint8_t actor_index;
extern uint32_t actor_space;
extern uint32_t actor_start;
extern uint32_t block_start;

/* actor create */
actor_t* actor_create(uint32_t taskStart, uint32_t taskLen, uint32_t result_len);
void edge_make(actor_t* src, actor_t* snk);
void packet_input(actor_t* actor, uint32_t data_addr, uint32_t data_len);
void assign_root(actor_t* actor);
void assign_sink(actor_t* actor);

#endif /* __TASK_H__ */
