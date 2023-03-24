#include "task.h"

/* SIMULATION ONLY */
uint8_t actor_index;
uint32_t actor_space;
uint32_t actor_start;

/* fireCheck.c */
extern void ready_search(void);
extern void actor_check(void);
extern uint8_t fire_check(actor_t *g);

/* Function: Initialize all actor's list and ready actor's list */
void list_init(void){
  actor_l = create_list();
  ready_l = create_list();
}

/* DAG depict example */
void DAG_depict() {
  printf(GREEN("\nSCHEDULER: Creating task...\n"));
  /* means to specify a actor (node) */
  actor_t *A = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t *B = actor_create(TASK2_START, TASK2_LEN, 4);
  actor_t *C = actor_create(TASK3_START, TASK3_LEN, 4);
  actor_t *D = actor_create(TASK3_START, TASK3_LEN, 4);
  actor_t *E = actor_create(TASK1_START, TASK1_LEN, 4);
  actor_t *F = actor_create(TASK2_START, TASK2_LEN, 4);
  actor_t *G = actor_create(TASK3_START, TASK3_LEN, 4);
  /* meas to specify dependencies */
  edge_make(A, B);
  edge_make(A, C);
  edge_make(A, D);
  edge_make(B, D);
  edge_make(B, F);
  edge_make(C, D);
  edge_make(C, E);
  edge_make(D, G);
  edge_make(E, G);
  edge_make(F, G);
  /* means to specify root and sink node of DAG */
  assign_root(A);
  assign_sink(G);
  /* means to input initial data packets */
  void *p = malloc(sizeof(int));
  *(uint32_t *)p = 10;
  packet_input(A, (uint32_t)p, 4);
}

/* SIMULATION ONLY: locate which task execution */
void para_init(void) {
  actor_start = (uint32_t)(actor_l->tail->prev)->item;
  actor_space = (uint32_t)(actor_l->tail->prev->prev)->item -
                (uint32_t)(actor_l->tail->prev)->item;
}

/* Function: Catch initial satified actors */
void schedule_init(void) {
  ready_search();
}


void actor_launch(void) {
  list_init();
  DAG_depict();
  para_init();
  schedule_init();
  actor_check();
}

