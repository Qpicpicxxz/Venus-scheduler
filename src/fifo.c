#include "task.h"

/* Reset fifo */
void init_fifo(fifo_t* F) { F->ptr = 0; }
void init_queue(queue_t* F) { F->ptr = 0; }

/* Predictate */
inline uint32_t fifo_full(fifo_t* F) { return (fifo_size(F) == MAXFIFO); }
inline uint8_t fifo_empty(fifo_t* F) { return (WPTR(F) == RPTR(F)); }
inline uint8_t queue_full(queue_t* F) { return (queue_size(F) == MAXFIFO); }
inline uint8_t queue_empty(queue_t* F) { return (WPTR(F) == RPTR(F)); }

/* Get length: maximum length is 255 */
uint8_t fifo_size(fifo_t* F) {
  uint8_t size = WPTR(F) - RPTR(F);
  if (size < 0) {
    size += MAXFIFO;
  }
  return size;
}
uint8_t queue_size(queue_t* F) {
  if (WPTR(F) >= RPTR(F))
    return WPTR(F) - RPTR(F);
  else
    return MAXFIFO - (RPTR(F) - WPTR(F));
}

/* Push in a token */
void put_data(fifo_t* F, data_t* data) {
  if (fifo_full(F)) {
    printf(REDSET "SCHEDULER: Fifo is full!" RESET);
    printf(" pointer 0x%x write failed\n", data->ptr);
  } else {
    F->data[WPTR(F)] = data;
    F->ptr = (((WPTR(F) + 1) % MAXFIFO) << 16) | RPTR(F);
  }
}
void put_queue(queue_t* F, uint32_t ptr) {
  if (queue_full(F)) {
    printf(REDSET "SCHEDULER: Queue is full!" RESET);
    printf(" pointer 0x%x write failed\n", ptr);
  } else {
    F->addr = ptr;
    F->ptr = (((WPTR(F) + 1) % MAXFIFO) << 16) | RPTR(F);
  }
}

/* Get out a token */
data_t* get_data(fifo_t* F) {
  if (fifo_empty(F)) {
    return NULL;
  } else {
    data_t* data = F->data[RPTR(F)];
    F->ptr = (((RPTR(F) + 1) % MAXFIFO) & 0xFFFF) | (F->ptr & 0xFFFF0000);
    return data;
  }
}
uint32_t get_queue(queue_t* F) {
  if (queue_empty(F)) {
    return -1;
  } else {
    uint32_t r = F->addr;
    F->ptr = (((RPTR(F) + 1) % MAXFIFO) & 0xFFFF) | (F->ptr & 0xFFFF0000);
    return r;
  }
}

/* Read a token */
data_t* read_data(fifo_t* F) {
  if (RPTR(F) != WPTR(F)) {
    data_t* data = F->data[RPTR(F)];
    return data;
  }
  printf(RED("FIFO WRONG[rptr==wptr]\n"));
  return NULL;
}
data_t* read_else_data(fifo_t* F, uint8_t dist) {
  if (RPTR(F) != WPTR(F)) {
    return F->data[(RPTR(F) + dist) % MAXFIFO];
  }
  printf(RED("QUEUE WRONG[rptr==wptr]\n"));
  return NULL;
}

