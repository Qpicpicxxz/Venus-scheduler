#ifndef __DMA_CFG_H_
#define __DMA_CFG_H_

#define RESERVED_63         ((uint64_t)0 << 63)
#define DST_OSR_LMT(N)      ((uint64_t)N << 59)
#define SRC_OSR_LMT(N)      ((uint64_t)N << 55)
#define LOCK_CH_L(N)        ((uint64_t)N << 53)
#define LOCK_CH(N)          ((uint64_t)N << 52)
#define CH_PRIOR(N)         ((uint64_t)N << 49)
#define RESERVED_48         ((uint64_t)0 << 48)
#define DST_PER(N)          ((uint64_t)N << 44)
#define RESERVED_43         ((uint64_t)0 << 43)
#define SRC_PER(N)          ((uint64_t)N << 39)
#define DST_HWHS_POL(N)     ((uint64_t)N << 38)
#define SRC_HWHS_POL(N)     ((uint64_t)N << 37)
#define HS_SEL_DST(N)       ((uint64_t)N << 36)
#define HS_SEL_SRC(N)       ((uint64_t)N << 35)
#define TT_FC(N)            ((uint64_t)N << 32)
#define RESERVED_31_4       ((uint64_t)0 << 4)
#define DST_MULTBLK_TYPE(N) ((uint64_t)N << 2)
#define SRC_MULTBLK_TYPE(N) ((uint64_t)N << 0)

#endif /* __DMA_CFG_H_ */
