#include "task.h"

void random_DAG(void) {
  /* means to specify a actor (node) */
  actor_t* A = actor_create(TASK1_START, TASK1_LEN);
  actor_t* B = actor_create(TASK1_START, TASK1_LEN);
  actor_t* C = actor_create(TASK1_START, TASK1_LEN);
  actor_t* D = actor_create(TASK1_START, TASK1_LEN);
  /* meas to specify dependencies */
  edge_make(A, 0, B, 0);
  edge_make(A, 0, C, 0);
  edge_make(A, 0, D, 1);
  edge_make(A, 1, C, 1);
  edge_make(B, 0, D, 0);
  edge_make(C, 0, D, 2);
  edge_make(C, 1, D, 3);
  /* means to specify root and sink node of DAG */
  assign_root(A);
  assign_sink(D);
}

void random_DAG_dynamic_test(void) {
  /* means to specify a actor (node) */
  actor_t* A  = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R1 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R2 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R3 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R4 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R5 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R6 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R7 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R8 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* R9 = actor_create(TASK1_START, TASK1_LEN);
  actor_t* I  = actor_create(TASK1_START, TASK1_LEN);
  /* meas to specify dependencies */
  // edge_make(A, 0, I, 0);
  edge_make(A, 0, R1, 0);
  edge_make(A, 1, R2, 0);
  edge_make(A, 2, R3, 0);
  edge_make(A, 3, R4, 0);
  edge_make(A, 4, R5, 0);
  edge_make(A, 5, R6, 0);
  edge_make(A, 6, R7, 0);
  edge_make(A, 7, R8, 0);
  edge_make(A, 8, R9, 0);
  edge_make(R1, 0, I, 0);
  edge_make(R2, 0, I, 1);
  edge_make(R3, 0, I, 2);
  edge_make(R4, 0, I, 3);
  edge_make(R5, 0, I, 4);
  edge_make(R6, 0, I, 5);
  edge_make(R7, 0, I, 6);
  edge_make(R8, 0, I, 7);
  edge_make(R9, 0, I, 8);
  /* means to specify root and sink node of DAG */
  assign_root(A);
  assign_sink(I);
}

