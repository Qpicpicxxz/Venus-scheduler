#include "os.h"

extern uint32_t TASK1_START;
extern uint32_t TASK1_END;
//extern int M[];
extern int	blocktask_func1();
void os_main(void)
{
	
	int length;
	length = TASK1_END - TASK1_START;
	printf("TASK LEN: 0x%x\n", length);
	int i = blocktask_func1();
	printf("i: 0x%x\n", i);
	//M[]={0,1,2,3,4,5,6,7,8,9};
}

