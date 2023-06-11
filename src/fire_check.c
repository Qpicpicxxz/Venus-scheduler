#include "hw/addressmap.h"
#include "task.h"

list_t* actor_l; /* linked list for all actor */

extern block_t block_stru[MAX_NUM_CLUSTERS][MAX_NUM_BLOCKS];

/* task_callback.c  */
extern void callback(actor_t* g);
/* dma/dma_lli_create.c */
extern void dma_transfer_link(uint32_t dst, uint32_t src, uint32_t len, block_t* block, token_t* token);
/* hal.S */
extern uint32_t Mask_irq(uint32_t mask_irq);

/* Predicate: Check whether the actor statify the fire rules */
static inline uint32_t actor_ready(actor_t* actor) {
  // visit all dependencies' fifo
  for (int i = 0; actor->in[i] != NULL; i++) {
    if (fifo_empty(actor->in[i]))
      return 0;
  }
  return 1;
}

/* Predicate: Check whether descendants' buffer are full */
static inline uint32_t successor_full(actor_t* actor) {
  uint8_t flag = 1;
  for (int i = 0; actor->out[i][0] != NULL; i++) {
    for (int j = 0; actor->out[i][j] != NULL; j++)
      flag = flag && fifo_full(actor->out[i][j]);
  }
  return flag;
}

/* Function: Find an idle block */
static inline block_t* block_idle(void) {
  for (int i = 0; i < NUM_CLUSTERS; i++) {
    for (int j = 0; j < NUM_BLOCKS; j++) {
      if ((block_stru[i][j].flags & BLOCK_INFLIGHT) == 0) {
        return &block_stru[i][j];
      }
    }
  }
  return NULL;
}

// /* Funcition: Fire ready actors when VENUS has idle blocks */
// void actor_check(void) {
//   while (1) {
//     // search all the actors in DAG
//     for (node_t* p = actor_l->tail->prev; p != actor_l->head; p = p->prev) {
//       actor = (actor_t*)p->item;
//       // find a prepared actor
//       if (actor_ready() && !successor_full() && block_idle()) {

//         _set_block_flag(block, BLOCK_INFLIGHT);
//         // add_firelist();  // disable judge disorder function for now
//         // task bind
//         block->actor = actor;
//         // a real task, inform dma
//         uint32_t offset = 0;
//         // task code
//         uint32_t mask_irq = Mask_irq(0xffffffff);  // disable all interrupts
//         dma_transfer_link(block->base_addr + BLOCK_ISPM_OFFSET, actor->task_addr, actor->task_len, NULL, NULL);
//         // task data
//         for (int i = 0; actor->in[i] != NULL; i++) {
//           // get the dependency out from fifo
//           token_t* token        = get_token(actor->in[i]);
//           block_t* pseudo_block = NULL;
//           if (actor->in[i + 1] == 0) {
//             if (INFORM_DMA_IS_VECTOR(token->attr)) {
//               // TODO: look up the Vector Register table
//             } else {
//               // this data is a scalar
//               uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + offset;
//               dma_transfer_link(scalar_offset, token->data->ptr, token->attr, block, token);
//             }
//           } else {
//             if (INFORM_DMA_IS_VECTOR(token->attr)) {
//               // TODO: look up the Vector Register table
//             } else {
//               // this data is a scalar
//               uint32_t scalar_offset = block->base_addr + BLOCK_SDSPM_OFFSET + offset;
//               dma_transfer_link(scalar_offset, token->data->ptr, token->attr, pseudo_block, token);
//             }
//           }
//           offset += token->attr;
//         }
//         Mask_irq(mask_irq);
//       }
//       if (p == actor_l->head->next)
//         p = actor_l->tail;
//     }
//   }
// }

/* Funcition: Fire ready actors when VENUS has idle blocks */
void actor_check(void) {
  while (1) {
    // traverse all the actors in DAG
    for (node_t* p = actor_l->tail->prev; p != actor_l->head; p = p->prev) {
      // catch a prepared actor
      actor_t* actor = (actor_t*)p->item;
      block_t* block = block_idle();
      // all dependencies prepared + output fifo isn't full + has idle block
      if (actor_ready(actor) && !successor_full(actor) && block != NULL) {
        // check if its a pseudo task
        if (read_token(actor->in[0]) == (token_t*)PSEUDO_TOKEN_LABEL) {
          // if it's a pseudo task, then get out all it's dependencies and do return a NULL ready_actor struct
          for (int i = 0; actor->in[i] != NULL; i++) {
            get_token(actor->in[i]);
          }
          // pass pseudo token to it's successor, skip block calculation step
          for (int i = 0; actor->out[i][0] != NULL; i++) {
            for (int j = 0; actor->out[i][j] != NULL; j++) {
              // create a pseudo_token
              token_t* pseudo_token = (token_t*)PSEUDO_TOKEN_LABEL;
              put_token(actor->out[i][j], pseudo_token);
            }
          }
        } else {
          // it is a real task, load it onto block
          _set_block_flag(block, BLOCK_INFLIGHT);
          // task bind
          block->actor = actor;
          // initialize transfer destination offset
          uint32_t offset       = 0;
          token_t* pseudo_token = (token_t*)PSEUDO_TOKEN_LABEL;  // TODO: do not idenfity a name for this
          block_t* pseudo_block = NULL;
          // inform DMA to transfer task's code
          dma_transfer_link(block->base_addr + BLOCK_ISPM_OFFSET, actor->task_addr, actor->task_len, NULL, NULL);
          // traverse actor's all depdencies
          for (int i = 0; actor->in[i] != NULL; i++) {
            // get the dependency out from fifo
            token_t* token = get_token(actor->in[i]);
            // initialize next token
            token_t* nxt_token = token;
            // catch next token's value
            if (actor->in[i + 1] != NULL)
              nxt_token = read_token(actor->in[i + 1]);
            // just transfer the real token data
            if (token != pseudo_token) {
              if (actor->in[i + 1] == NULL || nxt_token == pseudo_token) {
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
          }
        }
      }
      if (p == actor_l->head->next)
        p = actor_l->tail;
    }
  }
}

