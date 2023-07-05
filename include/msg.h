#ifndef __MSG_H__
#define __MSG_H__

#include "hw/config.h"
#include "lli.h"
#include "task.h"
#include "types.h"

typedef struct message {
  cluster_t* cluster;
  uint32_t token_array[MAX_DMA_TRANSMIT_BLOCK];
} msg_t;

#endif /* __MSG_H__ */

