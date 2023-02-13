#include "os.h"

extern uint32_t TEXT_START;
extern uint32_t TEXT_END;
extern uint32_t DATA_START;
extern uint32_t DATA_END;
extern uint32_t RODATA_START;
extern uint32_t RODATA_END;
extern uint32_t TASK1_START;
extern uint32_t TASK1_END;
extern uint32_t TASK2_START;
extern uint32_t TASK2_END;
extern uint32_t TASK3_START;
extern uint32_t TASK3_END;
extern uint32_t BSS_START;
extern uint32_t BSS_END;
extern uint32_t HEAP_START;
extern uint32_t HEAP_SIZE;

void mem_init()
{
  uint32_t TEXT = TEXT_END - TEXT_START;
  uint32_t RODATA = RODATA_END - RODATA_START;
  uint32_t DATA = DATA_END - DATA_START;
  uint32_t TASK1 = TASK1_END - TASK1_START;
  uint32_t TASK2 = TASK2_END - TASK2_START;
  uint32_t TASK3 = TASK3_END - TASK3_START;
  uint32_t BSS = BSS_END - BSS_START;
  printf("TEXT:    0x%x -> 0x%x", TEXT_START, TEXT_END);
  printf("    SIZE:   0X%x\n", TEXT);
  printf("RODATA:  0x%x -> 0x%x", RODATA_START, RODATA_END);
  printf("    SIZE:   0X%x\n", RODATA);
  printf("DATA:    0x%x -> 0x%x", DATA_START, DATA_END);
  printf("    SIZE:   0X%x\n", DATA);
  printf("TASK1:   0x%x -> 0x%x", TASK1_START, TASK1_END);
  printf("    SIZE:   0X%x\n", TASK1);
  printf("TASK2:   0x%x -> 0x%x", TASK2_START, TASK2_END);
  printf("    SIZE:   0X%x\n", TASK2);
  printf("TASK3:   0x%x -> 0x%x", TASK3_START, TASK3_END);
  printf("    SIZE:   0X%x\n", TASK3);
  printf("BSS:     0x%x -> 0x%x", BSS_START, BSS_END);
  printf("    SIZE:   0X%x\n", BSS);
  printf("");
}

