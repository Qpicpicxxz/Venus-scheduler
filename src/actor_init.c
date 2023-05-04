#include "task.h"

/* SIMULATION ONLY */
uint8_t actor_index;
uint32_t actor_space;
uint32_t actor_start;

/* fireCheck.c */
extern void ready_search(void);
extern void actor_check(void);
extern uint8_t fire_check(actor_t* g);

/* DAG.c */
extern void random_DAG(void);
extern void random_DAG_dynamic_test(void);
// int array[16] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

/* Function: Initialize all actor's list and ready actor's list */
void list_init(void) {
  actor_l = create_list();
  ready_l = create_list();
}

/* DAG depict example */
void DAG_depict() {
#ifdef DEBUG_SCHEDULER
  printf(GREEN("\nSCHEDULER: Creating task...\n"));
#endif
  // DAG with random dependencies
  // random_DAG();
  random_DAG_dynamic_test();
  /* means to input initial data packets */
  for (int i = 0; i < NUM_PACKET; i++) {
    // Note: It just create a void pointer to the actor input
    packet_input((actor_t*)(actor_l->tail->prev)->item, (uint32_t)malloc(sizeof(int)), SCALAR_LABEL);
  }
}

/* SIMULATION ONLY: locate which task execution */
void para_init(void) {
  actor_start = (uint32_t)(actor_l->tail->prev)->item;
  actor_space = (uint32_t)(actor_l->tail->prev->prev)->item - (uint32_t)(actor_l->tail->prev)->item;
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

