/* This is a test verison - 2023-05-29 */
#include "common.h"

volatile uint32_t* data_start = (uint32_t*)BLOCK_SDSPM;

typedef struct Matrix_uint8_codeAfterRM {
  uint16_t m;
  uint16_t n;
  uint8_t data[1][9876];
} Matrix_uint8_codeAfterRM;

typedef struct PSSCH_Para {
  uint8_t psschNum;           // 1
  uint8_t codeWordNum;        // 1
  uint16_t tbSize;            //=testpara.tbsize
  uint8_t layerNum;           // 2
  uint8_t modulation;         //=testpara.Modulation
  uint16_t rnti[2];           //[65534;65535]
  uint8_t rv;                 //=testpara.RV
  uint16_t RIV;               //=testpara.RIV
  uint8_t prbStart;           //=testpara.PRB_Start
  uint8_t prbNum;             //=testpara.N_PRB - 2
  uint8_t I_MCS;              //=testpara.I_MCS
  uint32_t NIR;               // 1e8
  uint8_t ce_window_ls_avg;   // 12,
  uint8_t ce_sliding_ls_avg;  // 1,
} PSSCH_Para;

/*
 * +---------------+ BLOCK_SDSPM
 * | codeBlockNum  |
 * +---------------+
 * | psschPara     |
 * +---------------+
 * | codeAfterRM_0 |
 * +---------------+ // [0]依赖
 * | codeBlockNum  | // 这些都是冗余的(可以规避不传更好...)
 * +---------------+
 * | psschPara     |
 * +---------------+
 * | codeAfterRM_1 |
 * +---------------+ // [1]依赖
 * | codeBlockNum  |
 * +---------------+
 * | psschPara     |
 * +---------------+
 * | codeAfterRM_2 |
 * +---------------+ // [2]依赖
 */

void main(void) {
  printf("HELLO INTERLEAVE!\n");
  // auxiliary pointer for parameter location
  uint32_t* int_pointer;
  PSSCH_Para* pssch_pointer;
  // parse parameter
  uint32_t codeBlockNum                   = data_start[0];
  PSSCH_Para* psschPara                   = (PSSCH_Para*)(data_start + 1);
  Matrix_uint8_codeAfterRM* codeAfterRM_0 = (Matrix_uint8_codeAfterRM*)(psschPara + 1);
  int_pointer                             = (uint32_t*)(codeAfterRM_0 + 1);
  pssch_pointer                           = (PSSCH_Para*)(int_pointer + 1);
  Matrix_uint8_codeAfterRM* codeAfterRM_1 = (Matrix_uint8_codeAfterRM*)(pssch_pointer + 1);
  int_pointer                             = (uint32_t*)(codeAfterRM_1 + 1);
  pssch_pointer                           = (PSSCH_Para*)(int_pointer + 1);
  Matrix_uint8_codeAfterRM* codeAfterRM_2 = (Matrix_uint8_codeAfterRM*)(pssch_pointer + 1);
  printf("codeBlockNum : %d\n", codeBlockNum);
  printf("codeAfterRM address: %p, %p, %p $stop\n", codeAfterRM_0, codeAfterRM_1, codeAfterRM_2);
}

