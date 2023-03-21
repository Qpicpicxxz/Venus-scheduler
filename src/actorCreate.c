/* API can provides productivity and facilitates rapid prototyping for developers */
#include "task.h"

/* API for actor create */
actor_t *actor_create(uint32_t taskStart, uint32_t taskLen, uint32_t result_len) {
  /* 1. Transforming functions into task definitions */
  actor_t *actor = malloc(sizeof(actor_t));
  // the result_len is in bytes
  actor->dep_num = 0;
  actor->nxt_num = 0;
  actor->result_len = result_len;
  actor->task_addr = taskStart;
  actor->task_len = taskLen;
  /* 2. Transforming function invocations into task linked constructs */
  insert(actor_l, create_node((uint32_t)actor));
  printf("Task 0x%x created...\n", actor);
  return actor;
}

static uint32_t node_make(fifo_t **array, fifo_t *fifo) {
  // loop through the array to find the first unused index
  for (int i = 0; i < MAXIO; i++) {
    if (array[i] == NULL) {
      // assign new fifo_t pointer to the unused index
      array[i] = fifo;
      return 1;
    }
  }
  return 0;
}

/* API for DAG dipict */
void edge_make(actor_t *dep, actor_t *suc) {
  fifo_t *fifo = malloc(sizeof(fifo_t));
  int success;
  success = node_make(dep->out, fifo);
  if (!success) {
    printf(RED("Dependency actor's fifo is full\n"));
  } else {
    dep->nxt_num += 1;
  }
  success = node_make(suc->in, fifo);
  if (!success) {
    printf(RED("Successor actor's fifo is full\n"));
  } else {
    suc->dep_num += 1;
  }
}

/* API for initial stimultor inject */
void packet_input(actor_t *actor, uint32_t data_addr, uint32_t data_len) {
  actor->dep_num = 1;
  data_t *data = malloc(sizeof(data_t));
  data->ptr = data_addr;
  data->len = data_len;
  data->cnt = actor->nxt_num;
  put_data(actor->in[0], data);
}

/* API to assign root actor of DAG */
void assign_root(actor_t *actor) {
  fifo_t *in_fifo = malloc(sizeof(fifo_t));
  actor->in[0] = in_fifo;
  actor->dep_num = 1;
}

/* API to assign sink actor of DAG */
void assign_sink(actor_t *actor) {
  fifo_t *out_fifo = malloc(sizeof(fifo_t));
  actor->out[0] = out_fifo;
  actor->nxt_num = 1;
}

