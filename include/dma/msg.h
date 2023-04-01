#ifndef __DMA_MSG_H__
#define __DMA_MSG_H__

#include "dma/lli.h"
#include "types.h"

typedef struct message {
  lli_t*   lli;
  uint32_t source_addr;
} msg_t;

#endif /* __DMA_MSG_H__ */
