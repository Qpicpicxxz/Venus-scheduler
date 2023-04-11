#include "task.h"

void random_DAG(void) {
  /* means to specify a actor (node) */
  actor_t* A = actor_create(TASK1_START, TASK1_LEN);
  actor_t* B = actor_create(TASK1_START, TASK1_LEN);
  actor_t* C = actor_create(TASK1_START, TASK1_LEN);
  actor_t* D = actor_create(TASK1_START, TASK1_LEN);
  /* meas to specify dependencies */
  edge_make(A, 0, B);
  edge_make(A, 0, C);
  edge_make(A, 0, D);
  edge_make(A, 1, C);
  edge_make(B, 0, D);
  edge_make(C, 0, D);
  edge_make(C, 1, D);
  /* means to specify root and sink node of DAG */
  assign_root(A);
  assign_sink(D);
}
