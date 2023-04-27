#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "common.h"
#include "hw/config.h"
#include "defs.h"
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
  list_t* fire_list;
  list_t* linger_list;
} actor_t; /* actor template */

typedef struct ready {
  uint32_t actor_addr;
  list_t* dep_list;
} ready_t; /* ready-actor template */

#endif /* __ACTOR_H__ */


