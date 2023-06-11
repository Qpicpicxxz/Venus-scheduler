#include "codeaddr.h"
#include "task.h"

#ifdef VERISON_1
void dag_create(void) {
  /* means to specify a actor (node) */
  actor_t* A = actor_create(TASKA_START, TASKA_LEN);
  actor_t* B = actor_create(TASKB_START, TASKB_LEN);
  actor_t* C = actor_create(TASKC_START, TASKC_LEN);
  actor_t* D = actor_create(TASKD_START, TASKD_LEN);
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

/* DAG depict example */
void DAG_depict() {
  // DAG with random dependencies
  dag_create();
  /* means to input initial data packets */
  int* p = (int*)malloc(sizeof(int));  // 分配一个int类型大小的内存空间
  *p     = 10;                         // 将10写入此内存
  packet_input((actor_t*)(actor_l->tail->prev)->item, (uint32_t)p, SCALAR_LABEL);
}
#endif

#ifdef VERISON_2
void dag_create(void) {
  /* means to specify a actor (node) */
  actor_t* A = actor_create(TASKA_START, TASKA_LEN);
  actor_t* B = actor_create(TASKB_START, TASKB_LEN);
  actor_t* C = actor_create(TASKC_START, TASKC_LEN);
  actor_t* D = actor_create(TASKD_START, TASKD_LEN);
  actor_t* E = actor_create(TASKE_START, TASKE_LEN);
  /* meas to specify dependencies */
  edge_make(A, 0, B, 0);
  edge_make(A, 3, B, 1);
  edge_make(A, 0, C, 0);
  edge_make(A, 1, C, 1);
  edge_make(A, 2, C, 2);
  edge_make(A, 3, D, 0);
  edge_make(B, 0, E, 0);
  edge_make(C, 0, E, 1);
  edge_make(D, 0, E, 2);
  /* means to specify root and sink node of DAG */
  assign_root(A);
  assign_sink(E);
}

/* DAG depict example */
void DAG_depict() {
  // DAG with random dependencies
  dag_create();
  /* means to input initial data packets */
  int* p = (int*)malloc(sizeof(int));  // 分配一个int类型大小的内存空间
  *p     = 10;                         // 将10写入此内存
  packet_input((actor_t*)(actor_l->tail->prev)->item, (uint32_t)p, SCALAR_LABEL);
}
#endif

