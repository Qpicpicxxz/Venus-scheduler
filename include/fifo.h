#ifndef __FIFO_H__
#define __FIFO_H__

#include "defs.h"

/* data descriptor */
typedef struct data {
  uint32_t ptr;  // where the data resides
  uint32_t cnt;  // lifecycle
} data_t;        /* data template */

typedef struct token {
  data_t* data;
  uint32_t attr;  // If it's a scalar, then attr means array index. If it's a vector, then attr means vector register list address
} token_t;        /* token template */

/* token-fifo descriptor between actors */
typedef struct fifo {
  token_t* token[MAXFIFO];  // data descriptor's pointer
  uint32_t wptr;
  uint32_t rptr;
} fifo_t; /* fifo template */

/* Reset fifo */
inline void init_fifo(fifo_t* F) { F->wptr = F->rptr = 0; }

/* Get length: maximum length is 255 */
inline uint8_t fifo_size(fifo_t* F) {
  uint8_t size = F->wptr - F->rptr;
  if (size < 0) {
    size += MAXFIFO;
  }
  return size;
}

/* Predictate */
inline uint8_t fifo_full(fifo_t* F) { return (fifo_size(F) == MAXFIFO - 1); }
inline uint8_t fifo_empty(fifo_t* F) { return (F->wptr == F->rptr); }

/* Push in a token */
inline void put_token(fifo_t* F, token_t* token) {
  if (fifo_full(F)) {
    printf("SCHEDULER: Fifo is full! pointer 0x%x write failed $stop\n", token->data->ptr);
  } else {
    F->token[F->wptr] = token;
    F->wptr           = (F->wptr + 1) % MAXFIFO;
  }
}

/* Get out a token */
inline token_t* get_token(fifo_t* F) {
  if (fifo_empty(F)) {
    return NULL;
  } else {
    token_t* token = F->token[F->rptr];
    F->rptr        = (F->rptr + 1) % MAXFIFO;
    return token;
  }
}

/* Read a token */
inline token_t* read_token(fifo_t* F) {
  if (F->rptr != F->wptr) {
    token_t* token = F->token[F->rptr];
    return token;
  }
  printf("FIFO WRONG[rptr==wptr]\n");
  return NULL;
}

#endif /* __FIFO_H__ */

