#include "ratematch/ratematch.h"
#include "ratematch/ratematch_dataset_input_0.h"

#define OUTPUT_NUM 2

volatile uint32_t* data_start = (uint32_t*)BLOCK_SDSPM;  // 定义一个指针指向起始地址

void main(void) {
  printf("HELLO FORNERRUNNER!\n");

  // 1. parse all the input data
  uint32_t pssch_G      = data_start[0];
  uint32_t rvIdx        = data_start[1];
  uint32_t codeBlockID  = data_start[2];
  uint32_t codeBlockNum = data_start[3];

  // for simulation, I assume above 4 parameter are input data of forerunner...
  // in this task, it just pass this data to the successor...

  // declare + allocate result[0]'s start address
  uint32_t retaddr0_len = sizeof(int) * 4;
  void* retaddr0        = malloc(retaddr0_len);
  // initialize result[0]'s start address
  uint32_t* data_block_ptr = (uint32_t*)retaddr0;
  *data_block_ptr++        = pssch_G;  // first assign, then self increment
  *data_block_ptr++        = rvIdx;
  *data_block_ptr++        = codeBlockID;
  *data_block_ptr          = codeBlockNum;
  
  // store result[0]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(0), (uint32_t)retaddr0);
  // mark the moving length
  // in this case, its [pssch_G + rvIdx + codeBlockID + codeBlockNum], 4 integers
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(0), retaddr0_len);

  // declare + allocate result[1]'s start address
  uint32_t retaddr1_len = sizeof(PSSCH_Para) + sizeof(Matrix_uint8_codeWord);
  void* retaddr1        = malloc(retaddr1_len);
  // initialize result[1]'s start address
  retaddr1 = &psschPara;

  // store result[1]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(1), (uint32_t)retaddr1);
  // PSSCH_Para struct + Matrix_uint8_codeWord struct
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(1), retaddr1_len);

  // 存放返回值的个数
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_NUMRETREG_OFFSET, OUTPUT_NUM);

  // 拉高中断，结束本block的task计算
  job_done_invoke();
}
