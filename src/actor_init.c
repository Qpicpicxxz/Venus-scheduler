#include "task.h"

/* fire_check.c */
extern void actor_check(void);

/* task/dag.c */
extern void DAG_depict(void);

/* Function: Initialize all actor's list and ready actor's list */
inline void list_init(void) {
  actor_l = create_list();
}

inline void actor_nxt_init(void) {
  for (node_t* p = actor_l->tail->prev; p != actor_l->head; p = p->prev) {
    actor_t* actor = (actor_t*)p->item;
    for (int i = 0; actor->out[i][0] != NULL; i++) {
      actor->task_nxt++;
    }
  }
}

void actor_launch(void) {
  list_init();
  DAG_depict();
  actor_nxt_init();
  actor_check();
}

