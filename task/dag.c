#include "codeaddr.h"
#include "task.h"

// void ratematch_interleaving_dag_create(void) {
//   /* means to specify a actor (node) */
//   actor_t* A  = actor_create(TASKA_START, TASKA_LEN);
//   actor_t* R1 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
//   actor_t* R2 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
//   // actor_t* R3 = actor_create(TASK1_START, TASK1_LEN);
//   // actor_t* R4 = actor_create(TASK1_START, TASK1_LEN);
//   // actor_t* R5 = actor_create(TASK1_START, TASK1_LEN);
//   // actor_t* R6 = actor_create(TASK1_START, TASK1_LEN);
//   // actor_t* R7 = actor_create(TASK1_START, TASK1_LEN);
//   // actor_t* R8 = actor_create(TASK1_START, TASK1_LEN);
//   // actor_t* R9 = actor_create(TASK1_START, TASK1_LEN);
//   actor_t* I = actor_create(INTERLEAVING_START, INTERLEAVING_LEN);
//   /* meas to specify dependencies */
//   edge_make(A, 0, I, 0);
//   edge_make(A, 1, R1, 0);
//   edge_make(A, 2, R2, 0);
//   // edge_make(A, 3, R3, 0);
//   // edge_make(A, 4, R4, 0);
//   // edge_make(A, 5, R5, 0);
//   // edge_make(A, 6, R6, 0);
//   // edge_make(A, 7, R7, 0);
//   // edge_make(A, 8, R8, 0);
//   // edge_make(A, 9, R9, 0);
//   edge_make(R1, 0, I, 1);
//   edge_make(R2, 0, I, 2);
//   // edge_make(R3, 0, I, 3);
//   // edge_make(R4, 0, I, 4);
//   // edge_make(R5, 0, I, 5);
//   // edge_make(R6, 0, I, 6);
//   // edge_make(R7, 0, I, 7);
//   // edge_make(R8, 0, I, 8);
//   // edge_make(R9, 0, I, 9);
//   /* means to specify root and sink node of DAG */
//   assign_root(A);
//   assign_sink(I);
//   /* means to specify dynamic dependencies actor */
//   assign_dynamic(I);
// }

void forerrunner_ratematching_dag_create(void) {
  actor_t* F  = actor_create(FORERUNNER_START, FORERUNNER_LEN);
  actor_t* R1 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
  // 4 scalars: pssch_G rvIdx codeBlockID codeBlockNum
  edge_make(F, 0, R1, 0);
  // 2 structs: PSSCH_Para Matrix_uint8_codeWord
  edge_make(F, 1, R1, 1);
  assign_root(F);
  assign_sink(R1);
}

typedef struct {
  uint32_t pssch_G;
  uint32_t rvIdx;
  uint32_t codeBlockID;
  uint32_t codeBlockNum;
} Inputdata_packet;

/* DAG depict example */
void DAG_depict() {
  // create a dag
  // ratematch_interleaving_dag_create();
  forerrunner_ratematching_dag_create();

  /* means to input initial data packets */
  Inputdata_packet* data_packet = malloc(sizeof(Inputdata_packet));
  data_packet->pssch_G          = 69120;
  data_packet->rvIdx            = 0;
  data_packet->codeBlockID      = 0;  // start from zero
  data_packet->codeBlockNum     = 8;  // dynamic dependencies
  packet_input((actor_t*)(actor_l->tail->prev)->item, (uint32_t)data_packet, sizeof(Inputdata_packet));
}

