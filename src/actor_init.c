#include "codeaddr.h"
#include "hw/addressmap.h"
#include "task.h"

/* dma/dma_lli_create.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, cluster_t* cluster, token_t* token);

/* mem.S */
extern uint32_t DAG_START;
extern uint32_t DAG_END;

/* init.c*/
extern cluster_t cluster_stru[MAX_NUM_CLUSTERS];
/*
 * L1 scheduler 要给L2 scheduler传的东西：
 *   1. 整个DAG图
 *   2. DAG图中需要的所有Task code 和相应的 input Data
 */

/* 让编译器自己初始化好DAG图的方法 */
__attribute__((section(".dag"))) static fifo_t Q0;
__attribute__((section(".dag"))) static fifo_t Q1;
__attribute__((section(".dag"))) static fifo_t Q2;
__attribute__((section(".dag"))) static fifo_t Q3;
__attribute__((section(".dag"))) static fifo_t Q4;
__attribute__((section(".dag"))) static fifo_t Q5;
__attribute__((section(".dag"))) static fifo_t Q6;
__attribute__((section(".dag"))) static fifo_t Q7;
__attribute__((section(".dag"))) static fifo_t Q8;
__attribute__((section(".dag"))) static fifo_t Q9;
__attribute__((section(".dag"))) static fifo_t Q10;
__attribute__((section(".dag"))) static fifo_t Q11;
__attribute__((section(".dag"))) static fifo_t Q12;
__attribute__((section(".dag"))) static fifo_t Q13;
__attribute__((section(".dag"))) static fifo_t Q14;
__attribute__((section(".dag"))) static fifo_t Q15;
__attribute__((section(".dag"))) static fifo_t Q16;
__attribute__((section(".dag"))) static fifo_t Q17;
__attribute__((section(".dag"))) static fifo_t Q18;
__attribute__((section(".dag"))) static fifo_t Q19;

// error: initializer element is not constant
__attribute__((section(".dag"))) volatile static actor_t F = {
    .in[0]     = &Q18,
    .out[0][0] = &Q0,
    .out[1][0] = &Q1,
    .out[2][0] = &Q2,
    .out[3][0] = &Q3,
    .out[4][0] = &Q4,
    .out[5][0] = &Q5,
    .out[6][0] = &Q6,
    .out[7][0] = &Q7,
    .out[8][0] = &Q8,
    // .task_addr = FORERUNNER_START,  // this attribute may change after moved to L2 scheduler
    // .task_len  = FORERUNNER_LEN,
    .task_nxt = 9,
};  // actor_create()
__attribute__((section(".dag"))) volatile static actor_t R0 = {
    .in[0]  = &Q0,
    .out[0] = &Q9,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
// __attribute__((section(".dag"))) volatile static actor_t R0 = {0};
__attribute__((section(".dag"))) volatile static actor_t R1 = {
    .in[0]    = &Q1,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t R2 = {
    .in[0]  = &Q2,
    .out[0] = &Q11,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t R3 = {
    .in[0]  = &Q3,
    .out[0] = &Q12,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t R4 = {
    .in[0]  = &Q4,
    .out[0] = &Q13,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t R5 = {
    .in[0]  = &Q5,
    .out[0] = &Q14,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t R6 = {
    .in[0]  = &Q6,
    .out[0] = &Q15,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t R7 = {
    .in[0]  = &Q7,
    .out[0] = &Q16,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t R8 = {
    .in[0]  = &Q8,
    .out[0] = &Q17,
    // .task_addr = RATEMATCH_START,
    // .task_len  = RATEMATCH_LEN,
    .task_nxt = 1,
};
__attribute__((section(".dag"))) volatile static actor_t I = {
    .in[0]     = &Q9,
    .in[1]     = &Q10,
    .in[2]     = &Q11,
    .in[3]     = &Q12,
    .in[4]     = &Q13,
    .in[5]     = &Q14,
    .in[6]     = &Q15,
    .in[7]     = &Q16,
    .in[8]     = &Q17,
    .out[0][0] = &Q19,
    // .task_addr = INTERLEAVE_START,
    // .task_len  = INTERLEAVE_LEN,
    .task_nxt = 1,
};

// void R_I_DAG_depict(void) {
//   // edge_make(F, 0, R0, 0)
//   F.out[0][0] = &Q0;
//   R0.in[0]    = &Q0;
//   F.out[1][0] = &Q1;
//   R1.in[0]    = &Q1;
//   F.out[2][0] = &Q2;
//   R2.in[0]    = &Q2;
//   F.out[3][0] = &Q3;
//   R3.in[0]    = &Q3;
//   F.out[4][0] = &Q4;
//   R4.in[0]    = &Q4;
//   F.out[5][0] = &Q5;
//   R5.in[0]    = &Q5;
//   F.out[6][0] = &Q6;
//   R6.in[0]    = &Q6;
//   F.out[7][0] = &Q7;
//   R7.in[0]    = &Q7;
//   F.out[8][0] = &Q8;
//   R8.in[0]    = &Q8;

//   R0.out[0][0] = &Q9;
//   I.in[0]      = &Q9;
//   R1.out[0][0] = &Q10;
//   I.in[1]      = &Q10;
//   R2.out[0][0] = &Q11;
//   I.in[2]      = &Q11;
//   R3.out[0][0] = &Q12;
//   I.in[3]      = &Q12;
//   R4.out[0][0] = &Q13;
//   I.in[4]      = &Q13;
//   R5.out[0][0] = &Q14;
//   I.in[5]      = &Q14;
//   R6.out[0][0] = &Q15;
//   I.in[6]      = &Q15;
//   R7.out[0][0] = &Q16;
//   I.in[7]      = &Q16;
//   R8.out[0][0] = &Q17;
//   I.in[8]      = &Q17;
// }

void cluster_launch(void) {
  //   vcs_stop();
  printf("DAG: %d, DATA: %d, RATEMATCH_CODE: %d, INTERLEAVE_CODE: %d\n", DAG_END - DAG_START, FORERUNNER_LEN, RATEMATCH_LEN, INTERLEAVE_LEN);
  //   printf("L1 start transmission\n");
  cluster_t* cluster = &cluster_stru[0];
  // 1. On load to cluster 0
  _set_cluster_flag(cluster, CLUSTER_DMA_LOAD);
  // 1.1 inform DMA to transform DAG graph
  dma_transfer_link(VENUS_CLUSTER_L2_SCHEDULER_ADDR(0), DAG_START, DAG_END - DAG_START, NULL, NULL);
  // 1.2 inform DMA to transform task code
  dma_transfer_link(VENUS_CLUSTER_L2_SCHEDULER_ADDR(0), FORERUNNER_START, FORERUNNER_LEN, NULL, NULL);
  dma_transfer_link(VENUS_CLUSTER_L2_SCHEDULER_ADDR(0), RATEMATCH_START, RATEMATCH_LEN, NULL, NULL);
  dma_transfer_link(VENUS_CLUSTER_L2_SCHEDULER_ADDR(0), INTERLEAVE_START, INTERLEAVE_LEN, cluster, NULL);
}

/* simulate result transfer back (not real data)
 * 1. size of psschPara = 32 byte
 * 2. size of codeWord = 18848 byte
 * 3. size of codeVerifaction = 9880 byte
 * 4. size of 4 scalar data = 16 byte
 */
void cluster_unload(cluster_t* cluster) {
  _set_cluster_flag(cluster, CLUSTER_DMA_UNLOAD);
  uint32_t data_len = 32 + 16 + 18848 + 9880;
  uint32_t alloc_addr;
  for (uint32_t i = 0; i < 9; i++) {
    alloc_addr = (uint32_t)malloc(data_len);
    if (i == 8)
      dma_transfer_link(alloc_addr, VENUS_CLUSTER_L2_SCHEDULER_ADDR(0) + i * data_len, data_len, cluster, NULL);
    else
      dma_transfer_link(alloc_addr, VENUS_CLUSTER_L2_SCHEDULER_ADDR(0) + i * data_len, data_len, NULL, NULL);
  }
  //   vcs_stop();
  // printf("$stop\n");
}

