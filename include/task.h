#ifndef __TASK_H__
#define __TASK_H__

#define NUM_BLOCKS 8
#define NUM_PACKET 2
#define MAX_BUFFER 8
#define MAX_DEP    10
#define MAX_NXT    10
#define MAX_RESULT 10

#define MAXFIFO (MAX_BUFFER + 1)  // to differentiate fifo-full and fifo-empty
#define MAXIN   MAX_DEP
#define MAXOUT  MAX_NXT
#define MAXRES  MAX_RESULT

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
actor_t* actor_create(uint32_t taskStart, uint32_t taskLen);
void edge_make(actor_t* src, uint8_t dep_index, actor_t* snk);
void packet_input(actor_t* actor, uint32_t data_addr, uint32_t data_len);
void assign_root(actor_t* actor);
void assign_sink(actor_t* actor);

#endif /* __TASK_H__ */

