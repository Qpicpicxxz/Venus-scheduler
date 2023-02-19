#include "os.h"
#include "saddr.h"
#include "daddr.h"

#define MAXFIFO 16
#define MAXIO 4
#define DELAY 500

typedef struct fifo
{
  uint32_t data[MAXFIFO]; // token storage
  uint32_t wptr;          // write pointer
  uint32_t rptr;          // read poinger
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
extern uint32_t get_fifo(fifo_t *F);
extern uint8_t fifo_size(fifo_t *F);

// dma && block
extern void dma_code(uint32_t i_spm_addr,uint32_t task_addr, uint32_t task_len);
extern void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);
extern void block_task1(actorio_t *g);
extern void block_task2(actorio_t *g);

// task rule
extern void task1(actorio_t *g_in, actorio_t *g_out);
extern void task2(actorio_t *g);

extern uint32_t TASK1_START;
extern uint32_t TASK1_END;
extern uint32_t TASK2_START;
extern uint32_t TASK2_END;
extern uint32_t TASK3_START;
extern uint32_t TASK3_END;
