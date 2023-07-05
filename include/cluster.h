#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include "defs.h"

#define CLUSTER_DMA_LOAD   (uint32_t)(1 << 0)
#define CLUSTER_DMA_UNLOAD (uint32_t)(1 << 1)

/*
 * We use a Cluster struct to handle cluster's attributes
 * -flags: clusters' status (number of dag running on it)
 * -base_addr: start address in venus soc's memory map
 */
typedef struct cluster {
  uint32_t flags;
  uint32_t dag_num;
  uint32_t base_addr;
} cluster_t;

static inline void _set_cluster_flag(cluster_t* cluster, uint32_t flags) { cluster->flags |= flags; }
static inline void _clear_cluster_flag(cluster_t* cluster, uint32_t flags) { cluster->flags &= ~flags; }
static inline void _reset_cluster_flag(cluster_t* cluster) { cluster->flags = 0; }

#endif /* __CLUSTER_H__ */

