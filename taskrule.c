#include "fifo.h"

extern uint32_t data3;
extern uint32_t data4;

// task1: in[0] in[1] --> out[0] out[0]
// task1_io = {{&q,&q},{&q}}
void task1(actorio_t *g_in, actorio_t *g_out)
{
  // check the firing condition
  if (fifo_size(g_in->in[0]) >= 1 && fifo_size(g_in->in[1]))
  {
    printf("\nTASK1: Firing...\n");
    // dma_code(dst, loc, len);
    dma_code(SPM1_ADDR, TASK1_START, TASK1_END - TASK1_START);
    // dma_data(dst, data, len) which data represents a pointer
    dma_data(DATA1_ADDR, get_fifo(g_in->in[0]), DATA1_LEN);
    dma_data(DATA2_ADDR, get_fifo(g_in->in[1]), DATA2_LEN);
    block_task1(g_in);
    data3 = get_fifo(g_in->out[0]);
    data4 = get_fifo(g_in->out[0]);
    printf("data1 + data2 = data3 = %d\n", data3);
    printf("data1 - data2 = data4 = %d\n", data4);
    put_fifo(g_out->in[0], (uint32_t)&data3);
    put_fifo(g_out->in[0], (uint32_t)&data4);
  }
}

// task2: in[0] in[0] --> out[0] out[1]
// task2_io = {{&q},{&q,&q}}
void task2(actorio_t *g)
{
  // check the firing condition
  if (fifo_size(g->in[0]) >= 2)
  {
    printf("\nTASK2: Firing...\n");
    dma_code(SPM2_ADDR, TASK2_START, TASK2_END - TASK2_START);
    // dma_data(dst, data, len) which data represents a pointer
    dma_data(DATA3_ADDR, get_fifo(g->in[0]), DATA3_LEN);
    dma_data(DATA4_ADDR, get_fifo(g->in[0]), DATA4_LEN);
    block_task2(g);
    printf("data3 + data3 = %d\n", get_fifo(g->out[0]));
    printf("data4 + data4 = %d\n", get_fifo(g->out[1]));
  }
}
