#include "types.h"
extern void irq_init(void);

void block_handler(uint32_t block_index){return;}
void dma_handler(void) {return;}
void start_kernel(void){ irq_init(); while(1){};}
