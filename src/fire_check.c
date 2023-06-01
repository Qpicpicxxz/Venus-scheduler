#include "hw/addressmap.h"
#include "task.h"

list_t* actor_l;       /* linked list for all actor   */
list_t* ready_l;       /* linked list for ready actor */
static actor_t* actor; /* current handle actor        */
static block_t* block; /* current block descriptor    */

extern block_t block_stru[MAX_NUM_CLUSTERS][MAX_NUM_BLOCKS];

/* task_callback.c  */
extern void callback(actor_t* g);
/* dma/dma_lli_create.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, token_t* token);
/* hal.S */
extern uint32_t Mask_irq(uint32_t mask_irq);

/* Predicate: Check whether the actor statify the fire rules */
static inline uint32_t actor_ready(void) {
  // visit all dependencies' fifo
  for (int i = 0; actor->in[i] != NULL; i++) {
    if (fifo_empty(actor->in[i]))
      return 0;
  }
  return 1;
}

/* Predicate: Check whether descendants' buffer are full */
static inline uint32_t successor_full(void) {
  uint8_t flag = 1;
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    for (int j = 0; actor->out[i][j] != NULL; j++)
      flag = flag && fifo_full(actor->out[i][j]);
  }
  return flag;
}

/* Function: Find an idle block */
uint32_t block_idle(void) {
  for (int i = 0; i < NUM_CLUSTERS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      if ((block_stru[i][j].flags & BLOCK_INFLIGHT) == 0) {
        block = &block_stru[i][j];
        return 1;
      }
    }
  }
  return 0;
}

/* Funcition: Fire ready actors when VENUS has idle blocks */
void actor_check(void) {
  while (1) {
    // search all the actors in DAG
    for (node_t* p = actor_l->tail->prev; p != actor_l->head; p = p->prev) {
      actor = (actor_t*)p->item;
      // find a prepared actor
      if (actor_ready() && !successor_full() && block_idle()) {
        _set_block_flag(block, BLOCK_INFLIGHT);
        // add_firelist();  // disable judge disorder function for now
        // task bind
        block->actor = actor;
        // a real task, inform dma
        uint32_t offset = 0;
        // task code
        uint32_t mask_irq = Mask_irq(0xffffffff);  // disable all interrupts
        dma_transfer_link(block->base_addr + BLOCK_ISPM_OFFSET, actor->task_addr, actor->task_len, NULL, NULL);
        // task data
        for (int i = 0; actor->in[i] != NULL; i++) {
          // get the dependency out from fifo
          token_t* token        = get_token(actor->in[i]);
          block_t* pseudo_block = NULL;
          if (actor->in[i + 1] == 0) {
            if (INFORM_DMA_IS_VECTOR(token->attr)) {
              // TODO: look up the Vector Register table
            } else {
              // this data is a scalar
              uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + offset;
              dma_transfer_link(scalar_offset, token->data->ptr, token->attr, block, token);
            }
          } else {
            if (INFORM_DMA_IS_VECTOR(token->attr)) {
              // TODO: look up the Vector Register table
            } else {
              // this data is a scalar
              uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + offset;
              dma_transfer_link(scalar_offset, token->data->ptr, token->attr, pseudo_block, token);
            }
          }
          offset += token->attr;
        }
        Mask_irq(mask_irq);
      }
      if (p == actor_l->head->next)
        p = actor_l->tail;
    }
  }
}

