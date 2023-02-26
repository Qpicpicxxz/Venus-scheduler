#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "os.h"
#include "block.h"
#include "saddr.h"
#include "daddr.h"

#define MAXFIFO 16
#define MAXIO 4
#define DELAY 500

typedef struct fifo
{
  uint32_t data[MAXFIFO]; // token storage
  uint32_t wptr;          // write pointer
  uint32_t rptr;          // read pointer
} fifo_t;

// we will support MAXIO inputs and outputs per actor
// every in/out is a fifo queue
typedef struct actorio
{
  fifo_t *in[MAXIO];
  fifo_t *out[MAXIO];
} actorio_t;

// fifo
extern void init_fifo(fifo_t *F);
extern void put_fifo(fifo_t *F, uint32_t d);
extern uint8_t fifo_size(fifo_t *F);		// the max fifo_size(uint8_t) is 255
extern uint32_t get_fifo(fifo_t *F);		// push one data out
extern uint32_t read_fifo(fifo_t *F);		// just read the margine data
extern uint32_t read_else_fifo(fifo_t *F, uint8_t dist);	// read the inside data
extern uint32_t *get_addr_fifo(fifo_t *F);	// < get_addr_fifo > represents the data pointer's location
extern uint32_t *read_addr_fifo(fifo_t *F);	// <*read_addr_fifo> represents the data itself and <read_addr_fifo> represents the pointer's address
extern uint32_t *read_elseaddr_fifo(fifo_t *F, uint8_t dist);	// read the inside data address

// dma && block
extern void dma_code(uint32_t i_spm_addr,uint32_t task_addr, uint32_t task_len);
extern void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);
// simulate the block's behaviour
extern void task1_exe(actorio_t *g, uint32_t n_block);
extern void task2_exe(actorio_t *g);
extern void task3_exe(actorio_t *g);

// task rule
extern void task1(actorio_t *g_in, actorio_t *g_out);
extern void task2(actorio_t *g_in, actorio_t *g_out);
extern void task3(actorio_t *g_in, actorio_t *g_out);

// marco from linker script to locate task code section
extern uint32_t TASK1_START;
extern uint32_t TASK1_END;
extern uint32_t TASK2_START;
extern uint32_t TASK2_END;
extern uint32_t TASK3_START;
extern uint32_t TASK3_END;

#endif /* __ACTOR_H__ */
