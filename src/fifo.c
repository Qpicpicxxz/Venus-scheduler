#include "task.h"

/* Reset fifo */
void init_fifo(fifo_t* F) { F->wptr = F->rptr = 0; }
void init_queue(queue_t* F) { F->wptr = F->rptr = 0; }

/* Predictate */
inline uint8_t fifo_full(fifo_t* F) { return (fifo_size(F) == MAXFIFO - 1); }
inline uint8_t fifo_empty(fifo_t* F) { return (F->wptr == F->rptr); }
inline uint8_t queue_full(queue_t* F) { return (queue_size(F) == MAXFIFO - 1); }
inline uint8_t queue_empty(queue_t* F) { return (F->wptr == F->rptr); }

/* Get length: maximum length is 255 */
uint8_t fifo_size(fifo_t* F) {
  uint8_t size = F->wptr - F->rptr;
  if (size < 0) {
    size += MAXFIFO;
  }
  return size;
}
uint8_t queue_size(queue_t* F) {
  uint8_t size = F->wptr - F->rptr;
  if (size < 0) {
    size += MAXFIFO;
  }
  return size;
}

/* Push in a token */
void put_data(fifo_t* F, data_t* data) {
  if (fifo_full(F)) {
  } else {
    F->data[F->wptr] = data;
    F->wptr = (F->wptr + 1) % MAXFIFO;
  }
}
void put_queue(queue_t* F, uint32_t ptr) {
  if (queue_full(F)) {
  } else {
    F->addr = ptr;
    F->wptr = (F->wptr + 1) % MAXFIFO;
  }
}

/* Get out a token */
data_t* get_data(fifo_t* F) {
  if (fifo_empty(F)) {
    return NULL;
  } else {
    data_t* data = F->data[F->rptr];
    F->rptr = (F->rptr + 1) % MAXFIFO;
    return data;
  }
}
uint32_t get_queue(queue_t* F) {
  if (queue_empty(F)) {
    return -1;
  } else {
    uint32_t r = F->addr;
    F->rptr = (F->rptr + 1) % MAXFIFO;
    return r;
  }
}

/* Read a token */
data_t* read_data(fifo_t* F) {
  if (F->rptr != F->wptr) {
    data_t* data = F->data[F->rptr];
    return data;
  }
  return NULL;
}
data_t* read_else_data(fifo_t* F, uint8_t dist) {
  if (F->rptr != F->wptr) {
    return F->data[(F->rptr + dist) % MAXFIFO];
  }
  return NULL;
}

