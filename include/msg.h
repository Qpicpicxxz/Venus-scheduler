#ifndef __MSG_H__
#define __MSG_H__

#include "lli.h"
#include "task.h"
#include "types.h"

typedef struct message {
  lli_t* lli;
  block_t* block;
  list_t* token_list;
} msg_t;

#endif /* __MSG_H__ */

