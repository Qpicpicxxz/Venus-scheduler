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
void put_token(fifo_t* F, token_t* token) {
  if (fifo_full(F)) {
    printf(REDSET "SCHEDULER: Fifo is full!" RESET);
    printf(" pointer 0x%x write failed\n", token->data->ptr);
  } else {
    F->token[F->wptr] = token;
    F->wptr           = (F->wptr + 1) % MAXFIFO;
  }
}
void put_queue(queue_t* F, uint32_t ptr) {
  if (queue_full(F)) {
    printf(REDSET "SCHEDULER: Queue is full!" RESET);
    printf(" pointer 0x%x write failed\n", ptr);
  } else {
    F->addr = ptr;
    F->wptr = (F->wptr + 1) % MAXFIFO;
  }
}

/* Get out a token */
token_t* get_token(fifo_t* F) {
  if (fifo_empty(F)) {
    return NULL;
  } else {
    token_t* token = F->token[F->rptr];
    F->rptr        = (F->rptr + 1) % MAXFIFO;
    return token;
  }
}
uint32_t get_queue(queue_t* F) {
  if (queue_empty(F)) {
    return -1;
  } else {
    uint32_t r = F->addr;
    F->rptr    = (F->rptr + 1) % MAXFIFO;
    return r;
  }
}

/* Read a token */
token_t* read_token(fifo_t* F) {
  if (F->rptr != F->wptr) {
    token_t* token = F->token[F->rptr];
    return token;
  }
  printf(RED("FIFO WRONG[rptr==wptr]\n"));
  return NULL;
}
token_t* read_else_token(fifo_t* F, uint8_t dist) {
  if (F->rptr != F->wptr) {
    return F->token[(F->rptr + dist) % MAXFIFO];
  }
  printf(RED("QUEUE WRONG[rptr==wptr]\n"));
  return NULL;
}

