#include "common.h"
extern void irq_init(void);

void block_handler(uint32_t block_index){return;}
void start_kernel(void){ irq_init(); while(1){};}
