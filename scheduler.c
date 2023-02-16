#include "os.h"

void schedule_main(void)
{
	printf("Scheduler task awake!\n");
	*task1 = TASK1_START;
	int task1_len = TASK1_END - TASK1_START;
	//memcpy(&i_spm_addr, task1, task1_len);
	if (numa_nodes > 1) {
        int policy = MPOL_BIND;
        unsigned long nodemask[2] = {1, 0};
        int result = set_mempolicy(policy, nodemask, 2 * sizeof(unsigned long));
        if (result == 0) {
            printf("Successfully set memory policy to prefer NUMA node 0\n");
        } else {
            printf("Error setting memory policy\n");
        }
    }
}

