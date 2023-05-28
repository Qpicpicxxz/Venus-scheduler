#include "ratematch/ratematch.h"
#include "ulib.h"

//=====================output data======================
struct Matrix_uint8_codeAfterRM codeAfterRM;

uint32_t venus_div_ceil(uint32_t dividend, uint32_t divisor) {
  uint8_t remainder_flag = (dividend % divisor) ? 1 : 0;
  return dividend / divisor + remainder_flag;
}

void RateMatch(struct Matrix_uint8_codeAfterRM* codeAfterRM, uint8_t index, uint8_t C, struct Matrix_uint8_codeWord* codeWord, struct PSSCH_Para* psschPara,
               uint32_t G, uint8_t rvIdx) {
  uint8_t Qm, layerNum;
  Qm       = psschPara->modulation;
  layerNum = psschPara->layerNum;

  uint8_t QmNl, gamma;
  QmNl = Qm * layerNum;
  uint32_t G1;
  G1    = G / QmNl;
  gamma = G1 % C;
  uint16_t E;
  if (index < C - gamma) {
    E = QmNl * (G1 / C);
  } else {
    E = QmNl * (G1 / C + 1);
  }

  codeAfterRM->m = 1;
  codeAfterRM->n = E;

  CbRateMatching(codeAfterRM, index, codeWord, 1, E, rvIdx);
}

void CbRateMatching(struct Matrix_uint8_codeAfterRM* codeAfterRM, uint8_t index, struct Matrix_uint8_codeWord* encodedata, uint8_t Flag, uint16_t E,
                    uint8_t rvIdx) {
  // for (i=0;i<18432;i++)
  //     encodedata->data[0][i] = i;
  uint16_t FN     = encodedata->n;         // length of all 3 bitstreams(bit)
  uint16_t D      = FN / 3;                // length of each bitstream(bit)
  uint16_t C      = 32;                    // width of subblock interleaver
  uint16_t R      = venus_div_ceil(D, C);  // height of subblock interleaver
  uint16_t Krect  = R * C;                 // capacity of subblock interleaver(bit)
  uint16_t Dk     = Krect - D;             // Dummy bits number in one subblock
  uint16_t Dk_all = Dk * 3;                // Dummy bits number in all three subblocks
  uint16_t Ncb    = Krect * 3;
  uint16_t k0     = R * (2 * venus_div_ceil(Ncb, 8 * R) * rvIdx + 2);

  uint16_t row = k0 % R;
  uint16_t col = k0 / R;

  uint16_t k;
  // let P[?]= 3GPP interleaving sequence
  // let P_EXP[?]=[P[?],P[?]+32,P[?]+64]
  // Permute_Col = (P_EXP[?] % 32) * 3 + (P_EXP[?] / 32)
  uint8_t Permute_Col1[96] = {3, 51, 27, 75, 15, 63, 39, 87, 9, 57, 33, 81, 21, 69, 45, 93, 0, 48, 24, 72, 12, 60, 36, 84, 6, 54, 30, 78, 18, 66, 42, 90, 4, 52, 28, 76, 16, 64, 40, 88, 10, 58, 34, 82, 22, 70, 46, 94, 1, 49, 25, 73, 13, 61, 37, 85, 7, 55, 31, 79, 19, 67, 43, 91, 5, 53, 29, 77, 17, 65, 41, 89, 11, 59, 35, 83, 23, 71, 47, 95, 2, 50, 26, 74, 14, 62, 38, 86, 8, 56, 32, 80, 20, 68, 44, 92};
  uint8_t Permute_Col2[96] = {0, 48, 24, 72, 12, 60, 36, 84, 6, 54, 30, 78, 18, 66, 42, 90, 3, 51, 27, 75, 15, 63, 39, 87, 9, 57, 33, 81, 21, 69, 45, 93, 1, 49, 25, 73, 13, 61, 37, 85, 7, 55, 31, 79, 19, 67, 43, 91, 4, 52, 28, 76, 16, 64, 40, 88, 10, 58, 34, 82, 22, 70, 46, 94, 2, 50, 26, 74, 14, 62, 38, 86, 8, 56, 32, 80, 20, 68, 44, 92, 5, 53, 29, 77, 17, 65, 41, 89, 11, 59, 35, 83, 23, 71, 47, 95};
  uint8_t* Permute_Col;
  if (Flag == 0) {  // Convulutional
    Permute_Col = Permute_Col1;
  }       // Convulutional
  else {  // Turbo
    Permute_Col = Permute_Col2;
    uint16_t readindex;
    for (k = 0; k < E; k++) {
      if ((col * R + row) < Krect)  // systematic
      {
        readindex = row * 96 + Permute_Col[col];
        if (readindex < Dk_all)  // is dummy bit
          k--;
        else {
          codeAfterRM->data[0][k] = encodedata->data[0][readindex - Dk_all];
          // printf("readindex=%d, k=%d, row=%d, col=%d, lv1_P_col=%d, Permute_Col[col]=%d, data=%d\n",readindex,k,row,col,Permute_Col_2[col],Permute_Col[col],encodedata->data[0][readindex - Dk_all]);
        }
        row = (row + 1) % R;
        if (row == 0)
          col = col + 1;
      } else if (32 <= col && col <= 63)  // parity 1
      {
        readindex = row * 96 + Permute_Col[col];
        if (readindex < Dk_all)  // is dummy bit
          k--;
        else {
          codeAfterRM->data[0][k] = encodedata->data[0][readindex - Dk_all];
          // printf("readindex=%d, k=%d, row=%d, col=%d, lv1_P_col=%d, Permute_Col[col]=%d, data=%d\n",readindex,k,row,col,Permute_Col_2[col],Permute_Col[col],encodedata->data[0][readindex - Dk_all]);
        }
        row = row;
        col = col + 32;
      } else  // parity 2
      {
        readindex = (row * 96 + Permute_Col[col] + 3);
        if (readindex >= Ncb)
          readindex = 2;
        if (readindex < Dk_all)  // is dummy bit
          k--;
        else {
          codeAfterRM->data[0][k] = encodedata->data[0][readindex - Dk_all];
          // printf("readindex=%d, k=%d, row=%d, col=%d, lv1_P_col=%d, Permute_Col[col]=%d, data=%d\n",readindex,k,row,col,Permute_Col_2[col],Permute_Col[col],encodedata->data[0][readindex - Dk_all]);
        }
        row = (row + 1) % R;
        if (col == 95 && row == 0)
          col = 0;
        else if (row == 0)
          col = col - 31;
        else
          col = col - 32;
      }
    }
  }  // Turbo
}

static int uint8_t_verify(uint32_t n, const volatile uint8_t* test, const uint8_t* verify) {
  uint32_t i;
  // Unrolled for faster verification
  for (i = 0; i < n / 2 * 2; i += 2) {
    uint8_t t0 = test[i], t1 = test[i + 1];
    uint8_t v0 = verify[i], v1 = verify[i + 1];
    if (t0 != v0)
      return i + 1;
    if (t1 != v1)
      return i + 2;
  }
  if (n % 2 != 0 && test[n - 1] != verify[n - 1])
    return n;
  return 0;
}

#define OUTPUT_NUM 1
volatile uint32_t* data_start = (uint32_t*)BLOCK_SDSPM;  // 定义一个指针指向起始地址

// input_block_len:18444 output_block_len:8640 exec time on picorv32@300MHz:14.210ms num_instr:601610
void main(void) {
  uint32_t verifcation_result;

  // 1. parse all the input data
  uint32_t pssch_G      = data_start[0];  // 0x80100000
  uint32_t rvIdx        = data_start[1];  // 0x80100004
  uint32_t codeBlockID  = data_start[2];  // 0x80100008
  uint32_t codeBlockNum = data_start[3];  // 0x8010000c

  printf("running task%d_simplified_rate_match\n", codeBlockID);
  printf("%d, %d, %d, %d\n", pssch_G, rvIdx, codeBlockID, codeBlockNum);

  PSSCH_Para* psschPara                     = (PSSCH_Para*)(data_start + 4);  // data_start + 4 means BLOCK_SDSPM + 4 * sizeof(uint32_t)
  Matrix_uint8_codeWord* codeWord           = (Matrix_uint8_codeWord*)(psschPara + 1);
  Matrix_uint8_codeAfterRM* codeVerifcation = (Matrix_uint8_codeAfterRM*)(codeWord + 1);

  printf("&psschPara: %p, size: %d\n", psschPara, sizeof(PSSCH_Para));
  printf("&codeWord: %p, size: %d\n", codeWord, sizeof(Matrix_uint8_codeWord));
  printf("&codeVerifcation: %p, size: %d\n", codeVerifcation, sizeof(Matrix_uint8_codeAfterRM));  // 0x801000030

  // 2. do computation
  RateMatch(&codeAfterRM, codeBlockID, codeBlockNum, codeWord, psschPara, pssch_G, rvIdx);

  verifcation_result = uint8_t_verify(codeAfterRM.n, codeAfterRM.data[0], codeVerifcation->data[0]);
  printf("verify = %d\n", verifcation_result);
  printf("codeAfterRM.n = %d\n", codeAfterRM.n);
  if (codeBlockID == 2)
    printf("$stop\n");

  // store ratematch result
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETADDRREG_OFFSET(0), (uint32_t)(&verifcation_result));
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_RETLENREG_OFFSET(0), sizeof(verifcation_result));
  WRITE_BURST_32(BLOCK_CTRLREGS, VENUSBLOCK_NUMRETREG_OFFSET, OUTPUT_NUM);

  // pull block's interrupt, stop task computation
  job_done_invoke();
}
