#include "actor.h"

void init_fifo(fifo_t *F)
{
  F->wptr = F->rptr = 0;
}

// a method to put elements into the queue
void put_fifo(fifo_t *F, uint32_t d)
{
  if (((F->wptr + 1) % MAXFIFO) != F->rptr) // a circular queue
  {
    F->data[F->wptr] = d;
    F->wptr = (F->wptr + 1) % MAXFIFO;
  }
  else
  {
    printf("warning: fifo is full! token %d write failed\n", d);
  }
}

uint8_t fifo_size(fifo_t *F)
{
  if (F->wptr >= F->rptr)
    return F->wptr - F->rptr;
  else
    return MAXFIFO - (F->rptr - F->wptr) + 1;
}

// a method to get elements from the queue
uint32_t get_fifo(fifo_t *F)
{
  uint32_t r;
  if (F->rptr != F->wptr)
  {
    r = F->data[F->rptr];
    F->rptr = (F->rptr + 1) % MAXFIFO;
    return r;
  }
  return -1;
}

// a method to just read the margin data pointer
uint32_t read_fifo(fifo_t *F)
{
  if (F->rptr != F->wptr)
  {
    return F->data[F->rptr];
  }
  return -1;
}

uint32_t read_else_fifo(fifo_t *F, uint8_t dist)
{
  if (F->rptr != F->wptr)
  {
    return F->data[F->rptr + dist];
  }
  return -1;
}

// a method to get the margin data pointer
uint32_t *get_addr_fifo(fifo_t *F)
{
  uint32_t *a;
  if (F->rptr != F->wptr)
  {
    a = &(F->data[F->rptr]);
    F->rptr = (F->rptr + 1) % MAXFIFO;
    return a;
  }
  // may cause some warning, later to fix it
  return NULL;
}

// a method to just read the margin data pointer
uint32_t *read_addr_fifo(fifo_t *F)
{
  if (F->rptr != F->wptr)
  {
    return &(F->data[F->rptr]);
  }
  // may cause some warning, later to fix it
  return NULL;
}

// a method to just read inside data pointer
uint32_t *read_elseaddr_fifo(fifo_t *F, uint8_t dist)
{
  if (F->rptr != F->wptr)
  {
    return &(F->data[F->rptr + dist]);
  }
  return NULL;
}

