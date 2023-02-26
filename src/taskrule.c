#include "actor.h"

/* Note: The following data is simulated
	1. SPMx_ADDR  ---> we should know the mapping address of every block's spm --- <saddr.h>
	2. DATAx_ADDR ---> we should told DMA the destination of the data moving to the block --- <daddr.h>
	3. DATA_LEN   ---> we should get the length of every data segment --- <daddr.h>
	4. &datax_mem ---> we should allocate the memory in DDR for block's compute result
	5. block_task ---> we should have a handler to catch unoccupied blocks and schedule them
*/

static fifo_t q_block;

void block_recycle(block_f **block_n){
	put_fifo(&q_block, (uint32_t)block_n);
	//printf("0x%x\n", block_n);
}

// task1: in[0]  --> out[0]
void task1(actorio_t *g_in, actorio_t *g_out)
{
  // check the firing condition
  if (fifo_size(g_in->in[0]) >= 1 && fifo_size(&q_block)>=1)
  {
    printf("\nTASK1: Firing...\n");
    // catch the idle block
    int n_block = get_fifo(&q_block);
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    //printf("%d\n", _is_block_idle(&n_block));
    _set_block_flag(&n_block, BLOCK_BUSY);
    //printf("%d\n", _is_block_idle(&n_block));
    // dma_code(dst, loc, len);
    // Note: have to replace SPM1_ADDR which related to n_block
    dma_code(SPM1_ADDR, TASK1_START, TASK1_END - TASK1_START);
    // dma_data(dst, data, len) which data represents a pointer
    dma_data(DATA1_ADDR, get_fifo(g_in->in[0]), DATA1_LEN);
    // the block would notify the scheduler that the task has finished
    task1_exe(g_in, n_block);
    // scheduler obtain the result
    printf("in * in = out = %d\n", *(uint32_t *)read_fifo(g_in->out[0]));
    // pass the result data pointer to the successor
    put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  }
}

// task2: in[0] in[0] --> out[0]
void task2(actorio_t *g_in, actorio_t *g_out)
{
  // 1. check firing condition
  if (fifo_size(g_in->in[0]) >= 2)
  {
    printf("\nTASK2: Firing...\n");
    // 2. pass the task code's revelent information to DMA
    dma_code(SPM2_ADDR, TASK2_START, TASK2_END - TASK2_START);
    dma_data(DATA3_ADDR, get_fifo(g_in->in[0]), DATA3_LEN);
    dma_data(DATA4_ADDR, get_fifo(g_in->in[0]), DATA4_LEN);
    // 3. start block computing
    task2_exe(g_in);
    printf("in1 + in2 = out = %d\n", *(uint32_t *)read_fifo(g_in->out[0]));
    // 4. pass the result data pointer to the successor
    put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  }
}

// task3: in[0] in[0] --> out[0] out[0] out[0] out[0]
void task3(actorio_t *g_in, actorio_t *g_out)
{
  if (fifo_size(g_in->in[0]) >= 2)
  {
    printf("\nTASK3: Firing...\n");
    dma_code(SPM3_ADDR, TASK3_START, TASK3_END - TASK3_START);
    dma_data(DATA3_ADDR, get_fifo(g_in->in[0]), DATA3_LEN);
    dma_data(DATA4_ADDR, get_fifo(g_in->in[0]), DATA4_LEN);
    task3_exe(g_in);
    printf("in1 + in2 = out = %d\n", *(uint32_t *)read_fifo(g_in->out[0]));
    printf("in1 - in2 = out = %d\n", *(uint32_t *)read_else_fifo(g_in->out[0],1));
    printf("in1 * in2 = out = %d\n", *(uint32_t *)read_else_fifo(g_in->out[0],2));
    printf("in1 / in2 = out = %d\n", *(uint32_t *)read_else_fifo(g_in->out[0],3));
    put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
    put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
    put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
    put_fifo(g_out->in[0], (uint32_t)get_fifo(g_in->out[0]));
  }
}
