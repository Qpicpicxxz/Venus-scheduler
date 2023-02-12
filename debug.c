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

  printf("TEXT:      0x%x -> 0x%x\n", TEXT_START, TEXT_END);
  printf("RODATA:    0x%x -> 0x%x\n", RODATA_START, RODATA_END);
  printf("DATA:      0x%x -> 0x%x\n", DATA_START, DATA_END);
  printf("TASK1:     0x%x -> 0x%x\n", TASK1_START, TASK1_END);
  printf("TASK2:     0x%x -> 0x%x\n", TASK2_START, TASK2_END);
  printf("TASK3:     0x%x -> 0x%x\n", TASK3_START, TASK3_END);
  printf("BSS:       0x%x -> 0x%x\n", BSS_START, BSS_END);
  printf("");
}


