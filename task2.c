#include "common.h"

// assign common function into different task section
__attribute__((noinline, section(".task2"))) int
square2();
int square2(int a, int b)
{
  int c = add(a,b) * add(a,b);
  return c;
}
__attribute__((noinline, section(".task2"))) int blocktask2(void)
{
  int c = sub(2, 3);
  // ensure all the function called is in the same section
  int result = c + square2(3, 4);
  return result;
}

