#include "types.h"
#include "os.h"
#define MAXFIFO 3

typedef struct fifo
{
  uint32_t data[MAXFIFO]; // token storage
  uint32_t wptr;     // write pointer
  uint32_t rptr;     // read poinger
} fifo_t;

extern void init_fifo(fifo_t *F);
extern void put_fifo(fifo_t *F, uint32_t d);
extern uint32_t get_fifo(fifo_t *F);
extern uint8_t fifo_size(fifo_t *F);
