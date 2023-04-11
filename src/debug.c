#include "codeaddr.h"
#include "common.h"

extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t RODATA_START;
extern uint32_t RODATA_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;
extern uint32_t STACK_START;
extern uint32_t STACK_END;

void mem_info() {
  uint32_t TEXT = TEXT_END - TEXT_START;
  uint32_t RODATA = RODATA_END - RODATA_START;
  uint32_t DATA = DATA_END - DATA_START;
  uint32_t TASK1 = TASK1_END - TASK1_START;
  uint32_t TASK2 = TASK2_END - TASK2_START;
  uint32_t TASK3 = TASK3_END - TASK3_START;
  uint32_t BSS = BSS_END - BSS_START;
  printf("TEXT:    %p -> %p", TEXT_START, TEXT_END);
  printf("    SIZE:   %p\n", TEXT);
  printf("RODATA:  %p -> %p", RODATA_START, RODATA_END);
  printf("    SIZE:   %p\n", RODATA);
  printf("DATA:    %p -> %p", DATA_START, DATA_END);
  printf("    SIZE:   %p\n", DATA);
  printf("TASK1:   %p -> %p", TASK1_START, TASK1_END);
  printf("    SIZE:   %p\n", TASK1);
  printf("TASK2:   %p -> %p", TASK2_START, TASK2_END);
  printf("    SIZE:   %p\n", TASK2);
  printf("TASK3:   %p -> %p", TASK3_START, TASK3_END);
  printf("    SIZE:   %p\n", TASK3);
  printf("BSS:     %p -> %p", BSS_START, BSS_END);
  printf("    SIZE:   %p\n", BSS);
  printf("STACK:   %p -> %p", STACK_START, STACK_END);
  printf("    SIZE:   %p\n", STACK_SIZE);
  printf("");
}

