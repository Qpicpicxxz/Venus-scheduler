#ifndef __DMA_CTL_H_
#define __DMA_CTL_H_

#define SHADOWREG_OR_LLI_VALID(N) ((uint64_t)N << 63)
#define SHADOWREG_OR_LLI_LAST(N)  ((uint64_t)N << 62)
#define RESERVED_61_59            ((uint64_t)0 << 59)
#define IOC_BlkTfr(N)             ((uint64_t)N << 58)
#define DST_STAT_EN(N)            ((uint64_t)N << 57)
#define SRC_STAT_EN(N)            ((uint64_t)N << 56)
#define AWLEN(N)                  ((uint64_t)(N) << 48)
#define AWLEN_EN(N)               ((uint64_t)N << 47)
#define ARLEN(N)                  ((uint64_t)(N) << 39)
#define ARLEN_EN(N)               ((uint64_t)N << 38)
#define AW_PROT(N)                ((uint64_t)N << 35)
#define AR_PROT(N)                ((uint64_t)N << 32)
#define RESERVED_31               ((uint64_t)0 << 31)
#define NonPosted_LastWrite_En(N) ((uint64_t)N << 30)
#define AW_CACHE(N)               ((uint64_t)N << 26)
#define AR_CACHE(N)               ((uint64_t)N << 22)
#define DST_MSIZE(N)              ((uint64_t)N << 18)
#define SRC_MSIZE(N)              ((uint64_t)N << 14)
#define DST_TR_WIDTH(N)           ((uint64_t)N << 11)
#define SRC_TR_WIDTH(N)           ((uint64_t)N << 8)
#define RESERVED_7                ((uint64_t)0 << 7)
#define DINC(N)                   ((uint64_t)N << 6)
#define RESERVED_5                ((uint64_t)0 << 5)
#define SINC(N)                   ((uint64_t)N << 4)
#define RESERVED_3                ((uint64_t)0 << 3)
#define DMS(N)                    ((uint64_t)N << 2)
#define RESERVED_1                ((uint64_t)0 << 1)
#define SMS(N)                    ((uint64_t)N << 0);

#endif /* __DMA_CTL_H_ */
