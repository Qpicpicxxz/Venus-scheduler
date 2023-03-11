#ifndef __TASK_H__
#define __TASK_H__

#include "types.h"
#include "actor.h"
#include "block.h"
#include "daddr.h"
#include "os.h"
#include "saddr.h"


/* get block-task recycle function's pointer */
typedef void (*Taskfunc)(actor_t *g);

/* task fire rule: dependency checking & idle block checking */
extern void task(actor_t *g, block_f *n_block);

/* fifo */
extern void init_fifo(fifo_t *F);
extern void init_queue(queue_t *F);
extern uint8_t fifo_full(fifo_t *F);
extern uint8_t fifo_empty(fifo_t *F);
extern uint8_t queue_full(queue_t *F);
extern uint8_t queue_empty(queue_t *F);
extern uint8_t fifo_size(fifo_t *F);
extern uint8_t queue_size(queue_t *F);
extern void put_data(fifo_t *F, data_t *data);
extern void put_queue(queue_t *F, uint32_t ptr);
extern data_t *get_data(fifo_t *F);
extern uint32_t get_queue(queue_t *F);
extern data_t *read_data(fifo_t *F);
extern data_t *read_else_data(fifo_t *F, uint8_t dist);

/* dma */
extern void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len);
extern void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);
extern void dma_result(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);

/* For test:
 * 	1. Simulate block process and pass the data to the task
 *	2. Store data descriptor in it to simulate
 */
extern fifo_t dma_trans_in;
extern uint32_t result;

/* marco from linker script to locate task code section */
extern uint32_t TASK1_START;
extern uint32_t TASK1_END;
extern uint32_t TASK2_START;
extern uint32_t TASK2_END;
extern uint32_t TASK3_START;
extern uint32_t TASK3_END;

#endif /* __TASK_H__ */
