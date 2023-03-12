#ifndef __FIFO_H__
#define __FIFO_H__

#define MAXFIFO 8
#define MAXIO 4

/* data descriptor */
typedef struct data {
  uint32_t ptr; 	// where the data resides
  uint32_t len;		// data length (byte)
  uint8_t  cnt;		// data lifecycle
} data_t;	/* data template */

/* token-fifo descriptor between actors */
typedef struct fifo {
  data_t  *data[MAXFIFO]; // data descriptor's pointer
  uint8_t wptr;
  uint8_t rptr;
} fifo_t;	/* fifo template */

/* normal fifo-queue descriptor */
typedef struct queue {
  uint32_t addr;	// any data you want to represent
  uint8_t wptr;
  uint8_t rptr;
 } queue_t;	/* queue template */

#endif /* __FIFO_H__ */
