#ifndef __TASK_H__
#define __TASK_H__

#include "os.h"
#include "saddr.h"
#include "daddr.h"
#include "actor.h"
#include "block.h"

/* get block-task recycle function's pointer */
typedef void (*Taskfunc)(actorio_t *g_in, actorio_t *g_out, block_f *n_block);

/* fifo */
extern void init_fifo(fifo_t *F);
extern void put_fifo(fifo_t *F, uint32_t d);
extern uint8_t fifo_size(fifo_t *F);		// the max fifo_size(uint8_t) is 255
extern uint32_t get_fifo(fifo_t *F);		// push one data out
extern uint32_t read_fifo(fifo_t *F);		// just read the margine data
extern uint32_t read_else_fifo(fifo_t *F, uint8_t dist);	// read the inside data
extern uint32_t *get_addr_fifo(fifo_t *F);	// < get_addr_fifo > represents the data pointer's location
extern uint32_t *read_addr_fifo(fifo_t *F);	// <*read_addr_fifo> represents the data itself and <read_addr_fifo> represents the pointer's address
extern uint32_t *read_elseaddr_fifo(fifo_t *F, uint8_t dist);	// read the inside data address

/* dma */
extern void dma_code(uint32_t i_spm_addr,uint32_t task_addr, uint32_t task_len);
extern void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);

/* for test, simulate block process and pass the data to the task */
extern fifo_t dma_trans;

/* bind the task with block's flag */
extern void task1_bind(actorio_t *g, actorio_t *g_out, block_f *n_block);
extern void task2_bind(actorio_t *g, actorio_t *g_out, block_f *n_block);
extern void task3_bind(actorio_t *g, actorio_t *g_out, block_f *n_block);

/* task fire rule: dependency checking & idle block checking */
extern void task1(actorio_t *g_in, actorio_t *g_out);
extern void task2(actorio_t *g_in, actorio_t *g_out);
extern void task3(actorio_t *g_in, actorio_t *g_out);

/* marco from linker script to locate task code section */
extern uint32_t TASK1_START;
extern uint32_t TASK1_END;
extern uint32_t TASK2_START;
extern uint32_t TASK2_END;
extern uint32_t TASK3_START;
extern uint32_t TASK3_END;

#endif /* __TASK_H__ */
