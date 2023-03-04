#include "task.h"

void init_fifo(fifo_t *F)
{
  F->wptr = F->rptr = 0;
}

uint8_t fifo_size(fifo_t *F)
{
  if (F->wptr >= F->rptr)
    return F->wptr - F->rptr;
  else
    return MAXFIFO - (F->rptr - F->wptr) ;
}

/*
 * a method to put elements into the queue
 * due to condition checking, F->wptr cannot equal to F->rptr
 * so the max fifo size = MAXFIFO - 1
 */
void put_data(fifo_t *F, uint32_t data, uint32_t len)
{
  if (((F->wptr + 1) % MAXFIFO) != F->rptr) // a circular queue
  {
    F->data[F->wptr].ptr = data;
    F->data[F->wptr].len = len;
    F->wptr = (F->wptr + 1) % MAXFIFO;
  }
  else
  {
    printf("SCHEDULER: Fifo is full! pointer 0x%x write failed\n", data);
  }
}

// a method to get elements from the queue
data_t get_data(fifo_t *F)
{
  data_t r;
  data_t k;
  k.ptr = 0x00000000;
  if (F->rptr != F->wptr)
  {
    r = F->data[F->rptr];
    F->rptr = (F->rptr + 1) % MAXFIFO;
    return r;
  }
  return k;
}

uint32_t get_ptr(fifo_t *F)
{
  uint32_t r;
  if (F->rptr != F->wptr)
  {
    r = F->data[F->rptr].ptr;
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
    return F->data[F->rptr].ptr;
  }
  return -1;
}

uint32_t read_else_fifo(fifo_t *F, uint8_t dist)
{
  if (F->rptr != F->wptr)
  {
    return F->data[F->rptr + dist].ptr;
  }
  return -1;
}

// a method to get the margin data pointer
uint32_t *get_addr_fifo(fifo_t *F)
{
  uint32_t *a;
  if (F->rptr != F->wptr)
  {
    a = &(F->data[F->rptr].ptr);
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
    return &(F->data[F->rptr].ptr);
  }
  // may cause some warning, later to fix it
  return NULL;
}

// a method to just read inside data pointer
uint32_t *read_elseaddr_fifo(fifo_t *F, uint8_t dist)
{
  if (F->rptr != F->wptr)
  {
    return &(F->data[F->rptr + dist].ptr);
  }
  return NULL;
}

