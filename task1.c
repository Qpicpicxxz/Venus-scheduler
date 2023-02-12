#include "common.h"

__attribute__((noinline, section(".task1"))) int blocktask_func1(void)
{
  int i = 0;
  i++;
  int a = 3;
  int b = 5;
	// we can use the marco function in any section task
  int result = i + add(a, b);
  return result;
}

