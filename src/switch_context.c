#include "common.h"
#include "riscv.h"

/* task management */
struct context {
  /* ignore x0 */
  reg_t ra;
  reg_t sp;  // stack pointer
  reg_t gp;
  reg_t tp;
  reg_t t0;
  reg_t t1;
  reg_t t2;
  reg_t s0;  // frame pointer
  reg_t s1;
  reg_t a0;
  reg_t a1;
  reg_t a2;
  reg_t a3;
  reg_t a4;
  reg_t a5;
  reg_t a6;
  reg_t a7;

  // sx: callee-saved
  reg_t s2;
  reg_t s3;
  reg_t s4;
  reg_t s5;
  reg_t s6;
  reg_t s7;
  reg_t s8;
  reg_t s9;
  reg_t s10;
  reg_t s11;
  reg_t t3;
  reg_t t4;
  reg_t t5;
  reg_t t6;
};

/* switch.S */
extern void switch_to(struct context* next);
uint8_t task_stack[STACK_SIZE];
struct context ctx_task;

void sched_init() {
  w_mscratch(0);
}

/* Function: Create a thread */
void thread_create(void (*start_routin)(void)) {
  ctx_task.sp = (reg_t)&task_stack[STACK_SIZE - 1];
  ctx_task.ra = (reg_t)start_routin;
  struct context* next = &ctx_task;
  switch_to(next);
}

