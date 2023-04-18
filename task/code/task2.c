#include "common.h"

/*    TASK程序流程：
        1.访问0x8000_0000和0x8010_0000开始的地址，读取本模块的ISPM、DSPM，完成运算，在运算过程中可以向VenusBlock_ProgressReg寄存器写入一些值，方便scheduler获知task的执行情况。
        2.计算完成后，确定本task所需要返回的值/向量，如果数据较为零散，特别是标量分布得比较零散时，建议先行打包
        3.将所需要返回的值、向量、数据包的数量写入VenusBlock_NumRetReg，并在VenusBlock_RetAddr#N，VenusBlock_RetLen#N寄存器中写入各个返回值的所在地址与长度
        4.向VenusBlock_IntStatusReg寄存器的bit[0]写入1，使能本block的"task执行完毕中断"
        5.向VenusBlock_CfgReg寄存器bit[1]写入1，使能本block的"Waiting for Write Back Flag"
        6.向VenusBlock_CfgReg寄存器bit[0]写入0，使能本模块的soft reset，从而实现pico的自关闭
*/

void block_start(void) {
  printf("HELLO TASK2! $stop\n");
  job_done_callback();
}
