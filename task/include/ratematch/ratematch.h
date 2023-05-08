#ifndef __RATEMATCH_H__
#define __RATEMATCH_H__

#include "common.h"


typedef struct Matrix_uint8_codeWord {
  uint16_t m;
  uint16_t n;
  uint8_t data[1][18444];
} __attribute__((aligned(8))) Matrix_uint8_codeWord;

typedef struct Matrix_uint8_codeAfterRM {
  uint16_t m;
  uint16_t n;
  uint8_t data[1][9876];
} __attribute__((aligned(8))) Matrix_uint8_codeAfterRM;

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
} __attribute__((aligned(8))) PSSCH_Para;

uint32_t venus_ceil(uint32_t dividend, uint32_t divisor);

void RateMatch(struct Matrix_uint8_codeAfterRM* codeAfterRM, uint8_t index, uint8_t C, struct Matrix_uint8_codeWord* codeWord, struct PSSCH_Para* psschPara,
               uint32_t G, uint8_t rvIdx);

void CbRateMatching(struct Matrix_uint8_codeAfterRM* codeAfterRM,
                    uint8_t index, struct Matrix_uint8_codeWord* encodedata, uint8_t Flag, uint16_t E, uint8_t RV);

#endif /* __RATEMATCH_H__ */

