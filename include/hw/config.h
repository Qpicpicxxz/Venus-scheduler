#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#define NUM_PACKET             2
#define MAX_BUFFER             8
#define MAX_DEP                6
#define MAX_NXT                3
#define MAX_RESULT             12
#define MAX_DMA_TRANSMIT_BLOCK 16

// in actor_t, fifo_t* out[MAXRES][MAXOUT]
// MAXRES -> MAX_RESULT
// MAXOUT -> MAX_NXT

#define NUM_CLUSTERS 2
#define NUM_BLOCKS   2

#define MAX_NUM_CLUSTERS 16
#define MAX_NUM_BLOCKS   8

#endif /* __HW_CONFIG_H_ */

