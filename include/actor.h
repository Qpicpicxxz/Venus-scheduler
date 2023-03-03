#ifndef __ACTOR_H__
#define __ACTOR_H__

#define MAXFIFO 8
#define MAXIO 4
#define DELAY 500

/* 
 * we can't determine the length of the array at initialization
 * the size of the structure is not known until runtime
 */
typedef struct fifo
{
  uint32_t data[MAXFIFO]; // token storage
  uint8_t wptr;          // write pointer
  uint8_t rptr;          // read pointer
} fifo_t;

/* 
 * we will support MAXIO inputs and outputs per actor
 * every in/out is a fifo queue
 */
typedef struct actorio
{
  fifo_t *in[MAXIO];
  fifo_t *out[MAXIO];
} actorio_t;

#endif /* __ACTOR_H__ */
