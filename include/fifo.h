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
  uint8_t wptr;
  uint8_t rptr;
} fifo_t; /* fifo template */

/* normal fifo-queue descriptor */
typedef struct queue {
  uint32_t addr;  // any data you want to represent
  uint8_t wptr;
  uint8_t rptr;
} queue_t; /* queue template */

/* fifo */
void init_fifo(fifo_t* F);
void init_queue(queue_t* F);
uint8_t fifo_full(fifo_t* F);
uint8_t fifo_empty(fifo_t* F);
uint8_t queue_full(queue_t* F);
uint8_t queue_empty(queue_t* F);
uint8_t fifo_size(fifo_t* F);
uint8_t queue_size(queue_t* F);
void put_token(fifo_t* F, token_t* token);
void put_queue(queue_t* F, uint32_t ptr);
token_t* get_token(fifo_t* F);
uint32_t get_queue(queue_t* F);
token_t* read_token(fifo_t* F);
token_t* read_else_token(fifo_t* F, uint8_t dist);

#endif /* __FIFO_H__ */

