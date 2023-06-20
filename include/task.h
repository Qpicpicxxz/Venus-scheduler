#ifndef __TASK_H__
#define __TASK_H__

#define MAXFIFO                (MAX_BUFFER + 1)  // to differentiate fifo-full and fifo-empty
#define MAXIN                  (MAX_DEP + 1)
#define MAXOUT                 (MAX_NXT + 1)
#define MAXRES                 (MAX_RESULT + 1)
#define MAX_DMA_TRANSMIT_BLOCK (MAX_DMA_LLI_BLOCK + 2)

// if is a vector, then it represents a vreg table address, then the highest bit is always 1
// so we can use this attribute to judge whether it should be moved into sdspm or vdspm
#define INFORM_DMA_IS_VECTOR(x) ((x)&0x80000000)
// this flag is set in task_callback.c (alloc_result) functions,
// TODO: for scalar packeting, should consider how to reserve/distinguish scalar data index
// for now, I just have some scalar data, so I can give scalar index in fire_check.c-fire_check()functions
// so we have to explicit represent the dependent actor's dependent index
#define PSEUDO_TOKEN_LABEL        666
#define LAST_TOKEN                987

#include "actor.h"
#include "block.h"
#include "common.h"
#include "fifo.h"
#include "linkedlist.h"

extern list_t* actor_l;
extern list_t* ready_l;

/* actor create */
actor_t* actor_create(uint32_t taskStart, uint32_t taskLen);
void edge_make(actor_t* src, uint8_t dep_index, actor_t* snk, uint8_t snk_index);
void packet_input(actor_t* actor, uint32_t data_addr, uint32_t token_attr);
void assign_root(actor_t* actor);
void assign_sink(actor_t* actor);

#endif /* __TASK_H__ */

