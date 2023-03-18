#ifndef __TASK_H__
#define __TASK_H__

#include "types.h"
#include "fifo.h"
#include "block.h"
#include "actor.h"
#include "daddr.h"
#include "os.h"

#define NUM_BLOCKS 8

/* actor create */
extern actor_t *task_create(uint32_t taskStart, uint32_t taskLen, uint32_t result_len);
extern void edge_make(actor_t *dep, actor_t *suc);
extern void packet_input(actor_t *actor, uint32_t data_addr, uint32_t data_len);
extern void assign_root(actor_t *actor);
extern void assign_sink(actor_t *actor);

/* marco from linker script to locate task code section */
extern uint32_t TASK1_START;
extern uint32_t TASK1_END;
extern uint32_t TASK1_LEN;
extern uint32_t TASK2_START;
extern uint32_t TASK2_END;
extern uint32_t TASK2_LEN;
extern uint32_t TASK3_START;
extern uint32_t TASK3_END;
extern uint32_t TASK3_LEN;

#endif /* __TASK_H__ */
