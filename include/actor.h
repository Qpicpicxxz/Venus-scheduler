#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "common.h"
#include "defs.h"
#include "hw/config.h"
#include "task.h"

/*
 * This supports MAXIO inputs and outputs per actor
 * Every in/out is a fifo queue
 */
typedef struct actor {
  fifo_t* in[MAXIN];
  fifo_t* out[MAXRES][MAXOUT];
  uint32_t task_addr;
  uint32_t task_len;
  uint32_t task_nxt;
} actor_t; /* actor template */

#endif /* __ACTOR_H__ */

