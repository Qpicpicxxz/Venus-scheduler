#include "task.h"

/* Reset fifo */
void init_fifo(fifo_t* F) { F->wptr = F->rptr = 0; }

/* Predictate */
inline uint8_t fifo_full(fifo_t* F) { return (fifo_size(F) == MAXFIFO - 1); }
inline uint8_t fifo_empty(fifo_t* F) { return (F->wptr == F->rptr); }

/* Get length: maximum length is 255 */
uint8_t fifo_size(fifo_t* F) {
  uint8_t size = F->wptr - F->rptr;
  if (size < 0) {
    size += MAXFIFO;
  }
  return size;
}

/* Push in a token */
void put_token(fifo_t* F, token_t* token) {
  if (fifo_full(F)) {
    printf("SCHEDULER: Fifo is full! pointer 0x%x write failed $stop\n", token->data->ptr);
  } else {
    F->token[F->wptr] = token;
    F->wptr           = (F->wptr + 1) % MAXFIFO;
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

/* Read a token */
token_t* read_token(fifo_t* F) {
  if (F->rptr != F->wptr) {
    token_t* token = F->token[F->rptr];
    return token;
  }
  printf("FIFO WRONG[rptr==wptr]\n");
  return NULL;
}

