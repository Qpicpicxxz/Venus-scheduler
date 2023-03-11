#ifndef __ACTOR_H__
#define __ACTOR_H__

#define MAXFIFO 8
#define MAXIO 4

typedef struct data {
  uint32_t ptr; 	// where the data resides
  uint32_t len;		// data length
  uint8_t  cnt;		// data lifecycle
} data_t; 		/* data template */

typedef struct fifo {
  data_t  *data[MAXFIFO]; // data pointer
  uint8_t wptr;         // write index
  uint8_t rptr;         // read index
} fifo_t;               /* fifo template */

typedef struct queue {
  uint32_t addr;
  uint8_t wptr;
  uint8_t rptr;
 } queue_t;

/*
 * This supports MAXIO inputs and outputs per actor
 * Every in/out is a fifo queue
 */
typedef struct actor {
  fifo_t *in[MAXIO];
  uint8_t dep_num;
  fifo_t *out[MAXIO];
  uint8_t nxt_num;
  uint32_t result_len;
  uint32_t task_addr;
  uint32_t task_len;
} actor_t; /* actor template */

#endif /* __ACTOR_H__ */
