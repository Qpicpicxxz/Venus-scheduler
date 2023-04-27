#include "task.h"

/* fire_check.c */
extern void ready_search(void);
extern void actor_check(void);
extern uint8_t fire_check(actor_t* g);

/* task/dag.c */
extern void DAG_depict(void);

/* Function: Initialize all actor's list and ready actor's list */
void list_init(void) {
  actor_l = create_list();
  ready_l = create_list();
}

/* Function: Catch initial satified actors */
void schedule_init(void) {
  ready_search();
}

void actor_launch(void) {
  list_init();
  DAG_depict();
  schedule_init();
  actor_check();
}

