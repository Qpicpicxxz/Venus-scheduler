#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "fifo.h"
#include "linkedlist.h"
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
  list_t *fire_list;
  list_t *linger_list;
} actor_t;	/* actor template */

typedef struct ready {
  uint32_t actor_addr;
  list_t *dep_addr;
} ready_t;	/* ready-actor template */

#endif /* __ACTOR_H__ */
