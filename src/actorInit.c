#include "task.h"

/* idle-block queue */
queue_t block_q;

/* linked list for all actor */
list_t *actor_l;

/* SIMULATION ONLY */
uint8_t actor_index;
uint32_t actor_space;
static uint32_t d1, d2, d3, d4, d5, d6;
/* task fire rule: dependency checking & idle block checking */
extern uint8_t task(actor_t *g, block_f *n_block);

/* SIMULATION ONLY: locate which task execution */
void para_init(void) {
  actor_space = (uint32_t)(actor_l->tail->prev->prev)->item -
                (uint32_t)(actor_l->tail->prev)->item;
}

/* SIMULATION ONLY: create original inputs */
void stimu_init(void){
  d1 = 10; // 730
  d2 = 5;  // 190
  d3 = 8;  // 472
  d4 = 3;  // 72
  d5 = 15; // 1620
  d6 = 6;  // 270
}

/* DAG depict example */
void actor_create(){
	stimu_init();
	printf(GREEN("\nSCHEDULER: Creating task...\n"));
	actor_t *A = task_create(TASK1_START,TASK1_LEN, 4);
	actor_t *B = task_create(TASK2_START,TASK2_LEN, 4);
	actor_t *C = task_create(TASK3_START,TASK3_LEN, 4);
	actor_t *D = task_create(TASK3_START,TASK3_LEN, 4);
	actor_t *E = task_create(TASK1_START,TASK1_LEN, 4);
	actor_t *F = task_create(TASK2_START,TASK2_LEN, 4);
	actor_t *G = task_create(TASK3_START,TASK3_LEN, 4);
	edge_make(A,B);
	edge_make(A,C);
	edge_make(A,D);
	edge_make(B,D);
	edge_make(B,F);
	edge_make(C,D);
	edge_make(C,E);
	edge_make(D,G);
	edge_make(E,G);
	edge_make(F,G);
	assign_root(A);
	assign_sink(G);
	packet_input(A, (uint32_t)&d1, 4);
	// packet_input(A, (uint32_t)&d2, 4);
	// packet_input(A, (uint32_t)&d3, 4);
	// packet_input(A, (uint32_t)&d4, 4);
	// packet_input(A, (uint32_t)&d5, 4);
	// packet_input(A, (uint32_t)&d6, 4);
}

/* Funcition: Poll tasks only when VENUS has idle blocks */
void actor_exe(void) {
  printf(GREEN("\nSCHEDULER: Waiting for blocks to be ready...\n"));
  // 1. initialize p to the last (first in) node
  link p = actor_l->tail->prev;
  while (1) {
    // 2. wait for idle blocks
    if (queue_size(&block_q) >= 1) {
      uint8_t fire = 0;
      // 3. idle blocks found, push out and use it
      block_f *n_block = (block_f *)get_queue(&block_q);
      // 5. polling for ready task
      while (!fire) {
        // 6. make list into a ring queue
        if (p == actor_l->head)
          p = actor_l->tail->prev;
        // 7. compute actor's index
        // Note: Careful of this cauculation...
        actor_index =
            ((uint32_t)p->item - (uint32_t)(actor_l->tail->prev)->item) /
            actor_space;
        printf(" %c", actor_index + 65);
        task_delay(10000);
        // 8. task dependency checking and catch if fire success or not
        fire = task((actor_t *)p->item, n_block);
        // 9. check next actor
        p = p->prev;
      }
    }
  }
}

void actor_launch(void) {
  actor_l = create_list();
  actor_create();
  para_init();
  actor_exe();
}

