#include "ratematch/ratematch.h"
#include "ratematch/ratematch_dataset_input_0.h"
#include "ratematch/ratematch_dataset_input_1.h"
#include "ratematch/ratematch_dataset_input_2.h"

#define OUTPUT_NUM 6

volatile uint32_t* data_start = (uint32_t*)BLOCK_SDSPM;  // 定义一个指针指向起始地址

void main(void) {
  printf("HELLO FORNERRUNNER!\n");

  // 1. parse all the input data
  uint32_t pssch_G_0      = data_start[0];
  uint32_t rvIdx_0        = data_start[1];
  uint32_t codeBlockID_0  = data_start[2];
  uint32_t codeBlockNum_0 = data_start[3];

  uint32_t pssch_G_1      = data_start[4];
  uint32_t rvIdx_1        = data_start[5];
  uint32_t codeBlockID_1  = data_start[6];
  uint32_t codeBlockNum_1 = data_start[7];

  uint32_t pssch_G_2      = data_start[8];
  uint32_t rvIdx_2        = data_start[9];
  uint32_t codeBlockID_2  = data_start[10];
  uint32_t codeBlockNum_2 = data_start[11];

  // for simulation, I assume above 4 parameter are input data of forerunner...
  // in this task, it just pass this data to the successor...

  // declare + allocate result[0]'s start address
  uint32_t retaddr0_len = sizeof(int) * 4;
  void* retaddr0        = malloc(retaddr0_len);
  // initialize result[0]'s start address
  uint32_t* data_block_ptr = (uint32_t*)retaddr0;
  *data_block_ptr++        = pssch_G_0;  // first assign, then self increment
  *data_block_ptr++        = rvIdx_0;
  *data_block_ptr++        = codeBlockID_0;
  *data_block_ptr          = codeBlockNum_0;

  // store result[0]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(0), (uint32_t)retaddr0);
  // mark the moving length
  // in this case, its [pssch_G + rvIdx + codeBlockID + codeBlockNum], 4 integers
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(0), retaddr0_len);

  // declare + allocate result[1]'s start address
  uint32_t retaddr1_len = sizeof(PSSCH_Para) + sizeof(Matrix_uint8_codeWord) + sizeof(Matrix_uint8_codeAfterRM);
  void* retaddr1        = malloc(retaddr1_len);
  // initialize result[1]'s start address
  retaddr1 = &psschPara_0;

  // store result[1]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(1), (uint32_t)retaddr1);
  // PSSCH_Para struct + Matrix_uint8_codeWord struct
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(1), retaddr1_len);

  uint32_t retaddr2_len = sizeof(int) * 4;
  void* retaddr2        = malloc(retaddr2_len);
  data_block_ptr        = (uint32_t*)retaddr2;
  *data_block_ptr++     = pssch_G_1;
  *data_block_ptr++     = rvIdx_1;
  *data_block_ptr++     = codeBlockID_1;
  *data_block_ptr       = codeBlockNum_1;
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(2), (uint32_t)retaddr2);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(2), retaddr2_len);
  uint32_t retaddr3_len = sizeof(PSSCH_Para) + sizeof(Matrix_uint8_codeWord) + sizeof(Matrix_uint8_codeAfterRM);
  void* retaddr3        = malloc(retaddr3_len);
  retaddr3              = &psschPara_1;
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(3), (uint32_t)retaddr3);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(3), retaddr3_len);

  uint32_t retaddr4_len = sizeof(int) * 4;
  void* retaddr4        = malloc(retaddr4_len);
  data_block_ptr        = (uint32_t*)retaddr4;
  *data_block_ptr++     = pssch_G_2;
  *data_block_ptr++     = rvIdx_2;
  *data_block_ptr++     = codeBlockID_2;
  *data_block_ptr       = codeBlockNum_2;
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(4), (uint32_t)retaddr4);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(4), retaddr4_len);
  uint32_t retaddr5_len = sizeof(PSSCH_Para) + sizeof(Matrix_uint8_codeWord) + sizeof(Matrix_uint8_codeAfterRM);
  void* retaddr5        = malloc(retaddr5_len);
  retaddr5              = &psschPara_2;
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(5), (uint32_t)retaddr5);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(5), retaddr5_len);

  // 存放返回值的个数
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_NUMRETREG_OFFSET, OUTPUT_NUM);

  // 拉高中断，结束本block的task计算
  job_done_invoke();
}

