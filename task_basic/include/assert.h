#ifndef __TASK_ASSERT_H_
#define __TASK_ASSERT_H_

#define assert(x)                                        \
  if (!(x)) {                                            \
    printf("\nAssertion failed: %s", #x);                \
    printf("\nFile: %s Line: %d\n", __FILE__, __LINE__); \
    printf("Please exit the program...\n");              \
    asm volatile("ebreak");                              \
  }

#endif /* __TASK_ASSERT_H_ */

