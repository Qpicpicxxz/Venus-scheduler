#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* In actor_t
 * fifo_t* in[MAXIN];
 * fifo_t* out[MAXRES][MAXOUT];
 * MAXIN  = MAX_DEP + 1
 * MAXOUT = MAX_NXT + 1
 * MAXRES = MAX_RESULT + 1
 */

#define NUM_PACKET        2   // initial inject number of packet
#define MAX_BUFFER        8   // depth of fifo
#define MAX_DEP           10  // max number of dependencies
#define MAX_NXT           3   // max number of each result's successor
#define MAX_RESULT        12  // max number of different result
#define MAX_DMA_LLI_BLOCK 16  // max support LLI number of each DMA channel in one transmission

// in actor_t, fifo_t* out[MAXRES][MAXOUT]
// MAXRES -> MAX_RESULT
// MAXOUT -> MAX_NXT

#define NUM_CLUSTERS 2
#define NUM_BLOCKS   2

#define MAX_NUM_CLUSTERS 16
#define MAX_NUM_BLOCKS   8

#endif /* __HW_CONFIG_H_ */

