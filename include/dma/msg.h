#ifndef __DMA_MSG_H__
#define __DMA_MSG_H__

#include "task.h"
#include "dma/lli.h"
#include "types.h"

typedef struct message {
  lli_t*   lli;
  block_t* block;
  list_t*  data_list;
} msg_t;

#endif /* __DMA_MSG_H__ */
