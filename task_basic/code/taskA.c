#include "common.h"

#ifdef VERISON_1
#define INPUT_SCALAR_NUM 1
#define OUTPUT_NUM       2
#endif

#ifdef VERISON_2
#define INPUT_SCALAR_NUM 1
#define OUTPUT_NUM       4
#endif

volatile uint32_t* input_data = (uint32_t*)BLOCK_SDSPM;  // 定义一个指针指向起始地址

#ifdef VERISON_1
void main(void) {
  printf("HELLO TASKA!\n");

  // 根据约定取得相应的数据进行计算
  uint32_t output_data_0 = input_data[0] + input_data[0];
  uint32_t output_data_1 = input_data[0] * input_data[0];
  printf("%d, %d\n", output_data_0, output_data_1);

  // 产生返回值[0]
  uint32_t* retaddr0 = (uint32_t*)malloc(sizeof(output_data_0));
  *retaddr0          = output_data_0;

  // 产生返回值[1]
  uint32_t* retaddr1 = (uint32_t*)malloc(sizeof(output_data_1));
  *retaddr1          = output_data_1;

  // 存放task A的返回值[0]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(0), (uint32_t)retaddr0);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(0), sizeof(output_data_0));

  // 存放task A的返回值[1]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(1), (uint32_t)retaddr1);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(1), sizeof(output_data_1));

  // 存放返回值的个数
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_NUMRETREG_OFFSET, OUTPUT_NUM);

  // 拉高中断，结束本block的task计算
  job_done_invoke();
}
#endif

#ifdef VERISON_2
void main(void) {
  printf("HELLO TASKA!\n");

  // 根据约定取得相应的数据进行计算
  uint32_t output_data_0 = input_data[0];
  uint32_t output_data_1 = input_data[0] * 2;
  uint32_t output_data_2 = input_data[0] * 3;
  uint32_t output_data_3 = input_data[0] * 4;
  printf("%d, %d, %d, %d\n", output_data_0, output_data_1, output_data_2, output_data_3);

  // 产生返回值[0]
  uint32_t* retaddr0 = (uint32_t*)malloc(sizeof(output_data_0));
  *retaddr0          = output_data_0;

  // 产生返回值[1]
  uint32_t* retaddr1 = (uint32_t*)malloc(sizeof(output_data_1));
  *retaddr1          = output_data_1;

  // 产生返回值[2]
  uint32_t* retaddr2 = (uint32_t*)malloc(sizeof(output_data_2));
  *retaddr2          = output_data_2;

  // 产生返回值[3]
  uint32_t* retaddr3 = (uint32_t*)malloc(sizeof(output_data_3));
  *retaddr3          = output_data_3;

  // 存放task A的返回值[0]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(0), (uint32_t)retaddr0);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(0), sizeof(output_data_0));

  // 存放task A的返回值[1]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(1), (uint32_t)retaddr1);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(1), sizeof(output_data_1));

  // 存放task A的返回值[2]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(2), (uint32_t)retaddr2);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(2), sizeof(output_data_2));

  // 存放task A的返回值[3]
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(3), (uint32_t)retaddr3);
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(3), sizeof(output_data_3));

  // 存放返回值的个数
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_NUMRETREG_OFFSET, OUTPUT_NUM);

  // 拉高中断，结束本block的task计算
  job_done_invoke();
}
#endif

