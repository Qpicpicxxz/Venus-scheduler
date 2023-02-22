#include "actor.h"

/* Note: The following data is simulated
	1. SPMx_ADDR  ---> we should know the mapping address of every block's spm --- <saddr.h>
	2. DATAx_ADDR ---> we should told DMA the destination of the data moving to the block --- <daddr.h>
	3. DATA_LEN   ---> we should get the length of every data segment --- <daddr.h>
	4. &datax_mem ---> we should allocate the memory in DDR for block's compute result
	5. block_task ---> we should have a handler to catch unoccupied blocks and schedule them
*/

// intermediate result storage address
// suppose the scheduler know the starting address of the result data return from every block
static fifo_t data1_mem;
static fifo_t data2_mem;
static fifo_t data3_mem;

// task1: in[0]  --> out[0]
void task1(actorio_t *g_in, actorio_t *g_out)
{
  // check the firing condition
  if (fifo_size(g_in->in[0]) >= 1)
  {
    printf("\nTASK1: Firing...\n");
    // dma_code(dst, loc, len);
    dma_code(SPM1_ADDR, TASK1_START, TASK1_END - TASK1_START);
    // dma_data(dst, data, len) which data represents a pointer
    dma_data(DATA1_ADDR, get_fifo(g_in->in[0]), DATA1_LEN);
    // the block would notify the scheduler that the task has finished
    block_task1(g_in);
    // scheduler obtain the result ( it's data itself )
    printf("in * in = out = %d\n", read_fifo(g_in->out[0]));
    // !!!! do not just pass the result to the next one!!!!! allocate the memory address for these data!!!!
    // put the data back to the memory ( we use data_addr fifo temporarily at present, to simulate memory allocation )
    // 1. block return the data itself, we put it in a data fifo
    put_fifo(&data2_mem, get_fifo(g_in->out[0]));
    // 2. pass the data's address (fifo pointer) to the next actor's token
    put_fifo(g_out->in[0], (uint32_t)get_addr_fifo(&data2_mem));
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
    block_task2(g_in);
    printf("in1 + in2 = out = %d\n", read_fifo(g_in->out[0]));
    // 4. restore the block returned value to the DDR
    put_fifo(&data3_mem, get_fifo(g_in->out[0]));
    // 5. pass the data pointer(token) to the next actor
    put_fifo(g_out->in[0], (uint32_t)get_addr_fifo(&data3_mem));
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
    block_task3(g_in);
    put_fifo(&data1_mem, get_fifo(g_in->out[0]));
    put_fifo(&data1_mem, get_fifo(g_in->out[0]));
    put_fifo(&data1_mem, get_fifo(g_in->out[0]));
    put_fifo(&data1_mem, get_fifo(g_in->out[0]));
    printf("in1 + in2 = out = %d\n", read_fifo(&data1_mem));
    printf("in1 - in2 = out = %d\n", read_else_fifo(&data1_mem,1));
    printf("in1 * in2 = out = %d\n", read_else_fifo(&data1_mem,2));
    printf("in1s / in2 = out = %d\n", read_else_fifo(&data1_mem,3));
    put_fifo(g_out->in[0], (uint32_t)get_addr_fifo(&data1_mem));
    put_fifo(g_out->in[0], (uint32_t)get_addr_fifo(&data1_mem));
    put_fifo(g_out->in[0], (uint32_t)get_addr_fifo(&data1_mem));
    put_fifo(g_out->in[0], (uint32_t)get_addr_fifo(&data1_mem));
  }
}
