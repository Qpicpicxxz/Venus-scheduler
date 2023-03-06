#ifndef __ACTOR_H__
#define __ACTOR_H__

#define MAXFIFO 8
#define MAXIO 4

typedef struct data
{
  uint32_t ptr;
  uint32_t len;
} data_t;	/* data template */

// Note: I'm not sure if I should add data length into fifo descriptor or not
typedef struct fifo	
{
  data_t   data[MAXFIFO]; // data pointer
  uint8_t  wptr;          // write index
  uint8_t  rptr;          // read index
} fifo_t;	/* fifo template */

/* 
 * This supports MAXIO inputs and outputs per actor
 * Every in/out is a fifo queue
 */
typedef struct actor
{
  fifo_t   *in[MAXIO];
  uint8_t  dep_num;
  fifo_t   *out;
  uint32_t result_len;
  uint32_t task_addr;
  uint32_t task_len;
} actor_t;	/* actor template */

#endif /* __ACTOR_H__ */
