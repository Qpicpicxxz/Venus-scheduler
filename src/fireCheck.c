#include "task.h"

/* linked list for all actor */
list_t *actor_l;
/* linked list for ready actor */
list_t *ready_l;

/* taskCallback.c  */
extern void callback(actor_t *g);
/* block.c */
extern void block_sim(block_f *block);
extern fifo_t dma_trans_in;
/* dma.c */
extern void dma_code(uint32_t i_spm_addr, uint32_t task_addr, uint32_t task_len);
extern void dma_data(uint32_t data_dst, uint32_t data_addr, uint32_t data_len);

/* idle-block queue */
queue_t block_q;

/* Predicate: Check whether the actor statify the fire rules */
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

/* Predicate: Check whether descendants' buffer are full */
uint8_t successor_full(actor_t *g) {
  uint8_t flag = 1;
  for (int i = 0; i < g->nxt_num; i++) {
    flag = flag && fifo_full(g->out[i]);
  }
  return flag;
}

/* Predicate: Check whether the actor could fire in this iteration */ 
uint8_t fire_check(actor_t *g) { return actor_ready(g) && !successor_full(g); }

/* Function: A handler to bind in-flight actor with current block */
void task_bind(actor_t *g, block_f *n_block) {
  printf("\nBLOCK: Computing task...\n");
  n_block->actor = g;
}

/* Function: Tranverse all the actors in DAG to search for readys */
void ready_search(void) {
  // 1. initialize a search node
  link p = actor_l->tail->prev;
  printf(GREEN("\nChecking actor:"));
  // 2. search all the actors in DAG
  while (p != actor_l->head) {
    actor_index = ((uint32_t)p->item - (uint32_t)(actor_l->tail->prev)->item) / actor_space;
    actor_t *actor = (actor_t *)p->item;
    // 3. checking fire condition
    uint8_t satisfied = fire_check(actor);
    // 4. handle ready actors
    if (satisfied) {
      // 4.1 allocate this ready actor's descriptor space
      ready_t *ready = malloc(sizeof(ready_t));
      // 4.2 bind the actor's address
      ready->actor_addr = (uint32_t)actor;
      // 4.3 initialize ready actor's dependencies list
      ready->dep_addr = create_list();
      // 4.4 transverse actor's dependencies
      for (uint32_t i = 0; i < actor->dep_num; i++) {
        // 4.4.1 get the dependency out from fifo
        data_t *data = get_data(actor->in[i]);
        // 4.4.5 bind the dependency descriptor pointer
        insert(ready->dep_addr, create_node((uint32_t)data));
      }
      // 4.5 put current actor into ready list (ready_t descriptor)
      insert(ready_l, create_node((uint32_t)ready));
      printf(GREEN("%c✔  "), actor_index + 65);
    } else {
      printf(PINK("%c✘  "), actor_index + 65);
    }
    // 5. checking next actor
    p = p->prev;
  }
}

/* Function: Parse descriptor and inform DMA */
void actor_fire(ready_t *ready, block_f *n_block) {
  // 1. get an actor from ready actor list
  actor_t *g = (actor_t *)ready->actor_addr;
  actor_index =
      ((uint32_t)g - (uint32_t)(actor_l->tail->prev)->item) / actor_space;
  printf("\nSCHEDULER: Actor ");
  printf(GREEN("%c"), actor_index + 65);
  printf(" Fired\n");
  printf("SCHEDULER: 0x%x block is ready...\n", n_block);
  // 2. mark this block to be inflight status
  _set_block_flag(n_block, BLOCK_INFLIGHT);
  // 3. inform task code descriptor to DMA
  dma_code(n_block->spm_addr, g->task_addr, g->task_len);
  // 4. access each dependency in turn
  for (uint32_t i = 0; i < g->dep_num; i++) {
    // 4.1 get data descriptor
    data_t *data = (data_t *)ready->dep_addr->tail->prev->item;
    delete (ready->dep_addr->tail->prev);
    // 4.2 initialize data descriptor (drip into simulated spm queue)
    put_data(&dma_trans_in, data);
    // 4.3 inform data descriptor to DMA
    dma_data(DATA1_ADDR, data->ptr, data->len);
  }
  destroy(ready->dep_addr);
  free(ready);
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
  // NOTE: this step should invoke after DMA has already moved the code to blocks!!!
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
}

/* Funcition: Fire ready actors when VENUS has idle blocks */
void actor_check(void) {
  printf(GREEN("\nSCHEDULER: Waiting for blocks to be ready...\n"));
  while (1) {
    // 1. if there is any idle block
    if (queue_size(&block_q) >= 1) {
      // 2. get out this idle block from idle-queue
      block_f *n_block = (block_f *)get_queue(&block_q);
      // 3. is there is any ready actor
      if (!is_list_empty(ready_l)) {
        // 4. parse the ready actor descriptor
        ready_t *ready = (ready_t *)ready_l->tail->prev->item;
        // 5. remove this actor from ready list
        delete (ready_l->tail->prev);
        // 6. invoke function with ready actor descriptor to fire actor
        actor_fire(ready, n_block);
      }
    }
  }
}
