#define MAXFIFO 10

typedef struct fifo
{
  int data[MAXFIFO]; // token storage
  int wptr;     // write pointer
  int rptr;     // read poinger
} fifo_t;

extern void init_fifo(fifo_t *F);
extern void put_fifo(fifo_t *F, int d);
extern int get_fifo(fifo_t *F);
extern int fifo_size(fifo_t *F);
