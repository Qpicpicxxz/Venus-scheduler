/* API can provides productivity and facilitates rapid prototyping for developers */
#include "assert.h"
#include "task.h"

/* API for actor create */
actor_t* actor_create(uint32_t taskStart, uint32_t taskLen) {
  /* 1. Transforming functions into task definitions */
  actor_t* actor = malloc(sizeof(actor_t));
  memset(actor->in, 0, MAXIN * sizeof(fifo_t*));
  for (int i = 0; i < MAXRES; i++) {
    memset(actor->out[i], 0, MAXOUT * sizeof(fifo_t*));
  }
  actor->task_addr = taskStart;
  actor->task_len  = taskLen;
  /* 2. Transforming function invocations into task linked constructs */
  insert(actor_l, create_node((uint32_t)actor));
  return actor;
}

static void node_make_in(fifo_t** array, fifo_t* fifo, uint8_t snk_index) {
  assert(array[snk_index] == NULL);
  array[snk_index] = fifo;
}
static uint32_t node_make_out(fifo_t** array, fifo_t* fifo) {
  // loop through the array to find the first unused index
  for (int i = 0; i < MAXOUT; i++) {
    if (array[i] == NULL) {
      // assign new fifo_t pointer to the unused index
      array[i] = fifo;
      return 1;
    }
  }
  return 0;
}

/*
 * API for DAG dipict
 *   dep_index = 0 ~ (MAX_RESULT - 1)
 */
void edge_make(actor_t* src, uint8_t dep_index, actor_t* snk, uint8_t snk_index) {
  assert(dep_index < MAXRES);
  fifo_t* fifo = malloc(sizeof(fifo_t));
  int success;
  success = node_make_out(src->out[dep_index], fifo);
  if (!success) {
    printf("Dependency actor's fifo is full\n");
    printf("Actor: %p -> %p $stop\n", src, snk);
  }
  node_make_in(snk->in, fifo, snk_index);
}

/* API for initial stimultor inject */
void packet_input(actor_t* actor, uint32_t data_addr, uint32_t data_attr) {
  data_t* data = malloc(sizeof(data_t));
  data->ptr    = data_addr;
  data->attr   = data_attr;
  if (fifo_full(actor->in[0])) {
    // if input buffer is full
    printf("Input buffer overflowed, discard residual data... $stop\n");
    return;
  }
  put_data(actor->in[0], data);
}

/* API to assign root actor of DAG */
void assign_root(actor_t* actor) {
  fifo_t* in_fifo = malloc(sizeof(fifo_t));
  actor->in[0]    = in_fifo;
}

/* API to assign sink actor of DAG */
void assign_sink(actor_t* actor) {
  fifo_t* out_fifo = malloc(sizeof(fifo_t));
  // this means sink actor has only one output result
  actor->out[0][0] = out_fifo;
}

