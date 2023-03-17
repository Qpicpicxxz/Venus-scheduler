#include "task.h"

extern void callback(actor_t *g);
extern void block_sim(block_f *block);

/* A handler to bind inflight task with current block */
void task_bind(actor_t *g, block_f *n_block) {
  actor_t *actor = g;
  printf("\nBLOCK: Computing task...\n");
  n_block->actor = actor;
}

/*
 * Function:
 *	Check whether this task could be fired,
 *	  which means all the predecessors finished their execution,
 *	  so all the dependencies have been satisfied
 *	This is a Dataflow Process Networks (DPN), so actor firings are
 *	  garded by a set of firing rules which specifies when an actor 
 *	  can be fires. These firing rules specifies precisely the number
 *	  and the values of tokens that must be available on the input
 *	  ports to the fire actor.
 */
uint8_t actor_ready(actor_t *g) {
  uint8_t flag = 1;
  // visit all dependencies' fifo
  for (int i = 0; i < g->dep_num; i++) {
    flag = flag && !fifo_empty(g->in[i]);
  }
  if (flag) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t successor_full(actor_t *g) {
  uint8_t flag = 1;
  for (int i = 0; i < g->nxt_num; i++) {
    flag = flag && fifo_full(g->out[i]);
  }
  if (flag) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Function:
 *	1. The Top-level function of dependency-checking process
 *	2. Inform DMA move the code and data
 *	3. Associate current block and task
 *	5. Call task simulation progress
 *	4. Check dependencies' lifecycle, decide whether to free their memory
 */
uint8_t task(actor_t *g, block_f *n_block) {
  /*
   *  1.1 check firing condition
   *  1.2 catch idle block
   *  1.3 check the successor's fifo is full or not
   */
  if (actor_ready(g) && !successor_full(g)) {
    printf("\nSCHEDULER: Actor 0x%x Fired\n", g);
    printf("SCHEDULER: 0x%x block is ready...\n", n_block);
    // 2. mark this block to be inflight status
    _set_block_flag(n_block, BLOCK_INFLIGHT);
    // 3. inform task code descriptor to DMA
    dma_code(n_block->spm_addr, g->task_addr, g->task_len);
    // 4. access each dependency in turn
    for (uint32_t i = 0; i < g->dep_num; i++) {
      // 4.1 get data descriptor
      data_t *data = get_data(g->in[i]);
      // 4.2 initialize data descriptor (drip into simulated spm queue)
      put_data(&dma_trans_in, data);
      // 4.3 inform data descriptor to DMA
      dma_data(DATA1_ADDR, data->ptr, data->len);
    }
    // 5. associate block and in-flight actor
    if (g->fire_list == NULL)
      g->fire_list = create_list();
    printf("SCHEDULER: Inserting block into in-flight list...\n");
    link p = create_node((uint32_t)n_block);
    insert(g->fire_list, p);
    // 6. associate block and task
    task_bind(g, n_block);
    // 7. JUST SIMULATION: compute result
    block_sim(n_block);
    // 8. DMA get out the dependency, check their lifecycle
    for (int i = 0; i < g->dep_num; i++) {
      data_t *data = get_data(&dma_trans_in);
      // 8.1 check dependency's lifecycle
      if (data->cnt == 1) {
        // 8.2 garbage collection
        printf("SCHEDULER: Last use, free data space...\n");
        free((void *)data->ptr); // free data space
        free((void *)data);      // free data flag space
      } else {
      	// 8.3 upate reference count (lifecycle)
        data->cnt--;
      }
    }
    return 1;
  } else {
    return 0;
  }
}
