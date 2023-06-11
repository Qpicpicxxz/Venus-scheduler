#include "codeaddr.h"
#include "task.h"

// void forerrunner_ratematching_dag_create(void) {
//   actor_t* F  = actor_create(FORERUNNER_START, FORERUNNER_LEN);
//   actor_t* R0 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
//   actor_t* R1 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
//   actor_t* R2 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
//   // 4 scalars: pssch_G rvIdx codeBlockID codeBlockNum
//   edge_make(F, 0, R0, 0);
//   // 2 structs: PSSCH_Para Matrix_uint8_codeWord
//   edge_make(F, 1, R0, 1);
//   edge_make(F, 2, R1, 0);
//   edge_make(F, 3, R1, 1);
//   edge_make(F, 4, R2, 0);
//   edge_make(F, 5, R2, 1);
//   assign_root(F);
//   assign_sink(R0);
//   assign_sink(R1);
//   assign_sink(R2);
// }

/* 测试场景：实际发射R0 R1 R2, 最多支持5个R */
void forerrunner_dag_dynamic(void) {
  actor_t* F  = actor_create(FORERUNNER_START, FORERUNNER_LEN);
  actor_t* R0 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
  actor_t* R1 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
  actor_t* R2 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
  actor_t* R3 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
  actor_t* R4 = actor_create(RATEMATCH_START, RATEMATCH_LEN);
  actor_t* I  = actor_create(INTERLEAVE_START, INTERLEAVE_LEN);
  // 4 scalars: pssch_G rvIdx codeBlockID codeBlockNum
  edge_make(F, 0, R0, 0);
  // 2 structs: PSSCH_Para Matrix_uint8_codeWord
  edge_make(F, 1, R0, 1);
  edge_make(F, 2, R1, 0);
  edge_make(F, 3, R1, 1);
  edge_make(F, 4, R2, 0);
  edge_make(F, 5, R2, 1);
  edge_make(F, 6, R3, 0);
  edge_make(F, 7, R3, 1);
  edge_make(F, 8, R4, 0);
  edge_make(F, 9, R4, 1);
  edge_make(R0, 0, I, 0);
  edge_make(R1, 0, I, 1);
  edge_make(R2, 0, I, 2);
  edge_make(R3, 0, I, 3);
  edge_make(R4, 0, I, 4);
  assign_root(F);
  assign_sink(I);
}

typedef struct {
  uint32_t pssch_G_0;
  uint32_t rvIdx_0;
  uint32_t codeBlockID_0;
  uint32_t codeBlockNum_0;

  uint32_t pssch_G_1;
  uint32_t rvIdx_1;
  uint32_t codeBlockID_1;
  uint32_t codeBlockNum_1;

  uint32_t pssch_G_2;
  uint32_t rvIdx_2;
  uint32_t codeBlockID_2;
  uint32_t codeBlockNum_2;
} Inputdata_packet;

/* DAG depict example */
void DAG_depict() {
  // create a dag
  // ratematch_interleaving_dag_create();
  forerrunner_dag_dynamic();

  /* means to input initial data packets */
  Inputdata_packet* data_packet = malloc(sizeof(Inputdata_packet));
  // dataset_0 scalars
  data_packet->pssch_G_0      = 69120;
  data_packet->rvIdx_0        = 0;
  data_packet->codeBlockID_0  = 0;  // start from zero
  data_packet->codeBlockNum_0 = 8;  // dynamic dependencies
  // dataset_1 scalars
  data_packet->pssch_G_1      = 69120;
  data_packet->rvIdx_1        = 0;
  data_packet->codeBlockID_1  = 1;  // start from zero
  data_packet->codeBlockNum_1 = 8;  // dynamic dependencies
  // dataset_2 scalars
  data_packet->pssch_G_2      = 69120;
  data_packet->rvIdx_2        = 0;
  data_packet->codeBlockID_2  = 2;  // start from zero
  data_packet->codeBlockNum_2 = 8;  // dynamic dependencies
  packet_input((actor_t*)(actor_l->tail->prev)->item, (uint32_t)data_packet, sizeof(Inputdata_packet));
}

