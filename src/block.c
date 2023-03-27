/* Simulate block's behaviour, DO NOT exist in real scheme. */

#include "task.h"

void block_sim(block_t* n_block) {
  // DO NOTHING...
  printf("BLOCK %d: Computing task %c...\n", ((uint32_t)n_block - block_start) / sizeof(block_t) +1 , actor_index + 65);
}

