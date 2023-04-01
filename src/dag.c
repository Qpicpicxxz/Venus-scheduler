#include "task.h"

void random_DAG(void){
  /* means to specify a actor (node) */
  actor_t* A1 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A2 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A3 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A4 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A5 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A6 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A7 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A8 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A9 = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t* A10 = actor_create(TASK1_START, TASK1_LEN, 4);
  /* meas to specify dependencies */
  edge_make(A1, A6);
  edge_make(A1, A8);
  edge_make(A2, A7);
  edge_make(A2, A9);
  edge_make(A3, A6);
  edge_make(A3, A7);
  edge_make(A4, A5);
  edge_make(A4, A8);
  edge_make(A4, A9);
  edge_make(A4, A10);
  edge_make(A5, A6);
  edge_make(A5, A7);
  edge_make(A5, A9);
  edge_make(A5, A10);
  edge_make(A6, A7);
  edge_make(A6, A8);
  edge_make(A6, A9);
  edge_make(A7, A8);
  edge_make(A1, A2);
  edge_make(A1, A3);
  edge_make(A1, A4);
  edge_make(A8, A10);
  edge_make(A9, A10);
  /* means to specify root and sink node of DAG */
  assign_root(A1);
  assign_sink(A10);
}