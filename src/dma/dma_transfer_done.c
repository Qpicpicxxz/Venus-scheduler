#include "common.h"
#include "dma.h"
#include "hw/axi.h"
#include "hw/config.h"
#include "msg.h"
#include "task.h"

/* init.c */
extern msg_t* msg_array[DMAC_NUMBER_OF_CHANNELS];
/* fire_check.c */
extern void ready_search(void);
// for test
/* actor_init.c */
extern void cluster_unload(cluster_t* cluster);

/* internal variables */
static cluster_t* cluster;
static msg_t* msg;

// static void scheduler_pass_result() {
//   actor_t* actor       = block->actor;
//   uint32_t token_index = 0;

//   // pass the result to successors i -> different result |  j -> different fifo
//   for (int i = 0; actor->out[i][0] != NULL; i++) {
//     // real token
//     if (msg->token_array[token_index] != LAST_TOKEN) {
//       for (int j = 0; actor->out[i][j] != NULL; j++) {
//         token_t* original_token = (token_t*)msg->token_array[token_index];
//         // if its the first fifo of this result
//         if (j == 0) {
//           put_token(actor->out[i][j], original_token);
//         } else {
//           // create(duplicate) a new data structure (to ensure different fifo has different data structure)
//           token_t* dup_token = malloc(sizeof(token_t));
//           dup_token->data    = original_token->data;
//           dup_token->attr    = original_token->attr;
//           put_token(actor->out[i][j], dup_token);
//         }
//       }
//       token_index++;
//     } else {
//       // pseudo token (dynamic dependencies)
//       for (int j = 0; actor->out[i][j] != NULL; j++) {
//         // create a pseudo_token and inject it into pseudo task's fifo
//         token_t* pseudo_token = (token_t*)PSEUDO_TOKEN_LABEL;
//         put_token(actor->out[i][j], pseudo_token);
//       }
//     }
//   }

//   // TODO: arrival disorder check...
// }

// static inline void recycle_garbage(void) {
//   token_t* token;

//   for (uint32_t token_index = 0; msg->token_array[token_index] != LAST_TOKEN; token_index++) {
//     // parse data struct
//     token = (token_t*)msg->token_array[token_index];
//     if (token->data->cnt == 1) {
//       // garbage collection
//       free((void*)token->data->ptr);  // free real data
//       free((void*)token->data);       // free data flag space
//     } else {
//       // upate reference count (lifecycle)
//       token->data->cnt--;
//     }
//     free((void*)token);
//   }
// }

void dma_transmit_done_handler(uint32_t channel_index) {
  /* catch current DMA's transfer information */
  msg     = msg_array[channel_index];
  cluster = msg->cluster;

  // TODO: handle DMA transmit done operation
  if ((cluster->flags & CLUSTER_DMA_UNLOAD) == 0) {
    // Loading DAG into L2 scheduler's memory...
    cluster->dag_num++;
    _clear_cluster_flag(cluster, CLUSTER_DMA_LOAD);
    // for test [2023-06-30]
    cluster_unload(cluster);
  } else {
    // Unloading DAG's result to main memory...
    cluster->dag_num--;
    _clear_cluster_flag(cluster, CLUSTER_DMA_UNLOAD);
    vcs_stop();
  }

  // /* judge whether code/data transfer OR result transfer */
  // if ((block->flags & BLOCK_RESULT) == 0) {
  //   // activate corresponding block (ASAP!!!)
  //   WRITE_BURST_32(block->base_addr, BLOCK_CTRLREGS_OFFSET, EN_SOFT_RST);
  //   recycle_garbage();
  // } else {
  //   // pass result to the successor
  //   scheduler_pass_result();
  //   // reset the block's status flag
  //   _clear_block_flag(block);
  // }

  /* In previous verison, we should free LLI descriptor's memory space
   * now we do not have to free these memory blocks
   */
}

