### **🦆描述符结构**
***
**fifo.h**
```
typedef struct data {
  uint32_t ptr
  uint32_t len
  uint32_t cnt
} data_t
```
```
typedef struct fifo {
  data_t* data[MAXFIFO]
  uint32_t ptr
} fifo_t
```
```
typedef struct queue {
  uint32_t addr
  uint32_t ptr
} queue_t
```
**linkedList.h**
```
typedef struct node {
  uint32_t item
  node_t* prev
  node_t* next
} node_t
```
```
typedef struct list {
  node_t* head
  node_t* tail
} list_t
```
**actor.h**
```
typedef struct actor {
  fifo_t* in[MAXIO]
  uint32_t dep_num
  fifo_t* out[MAXIO]
  uint32_t nxt_num
  uint32_t result_len
  uint32_t task_addr
  uint32_t task_len
  list_t* fire_list
  list_t* linger_list
} actor_t
```
```
typedef struct ready {
  uint32_t actor_addr
  list_t* dep_list
} ready_t
```
**block.h**
```
typedef struct block {
  uint32_t flags
  uint32_t spm_addr
  actor_t* actor
  uint32_t result
} block_t
```
```
typedef struct linger {
  block_t* block
  data_t* data
} linger_t
```
***
### **👾Tips: 设计struct的时候可以考虑结构体的占用空间**
```
struct stru_1{
  uint8_t a
  uint8_t b
}
sizeof(struct stru_1) == 2
```
```
struct stru_2{
  uint8_t a
  uint32_t b
}
sizeof(struct stru_2) == 8
```
```
struct stru_3{
  uint8_t a
  uint32_t b
  uint8_t c
}
sizeof(struct stru_3) == 12
```
```
struct stru_4{
  uint8_t a
  uint8_t b
  uint32_t c
}
sizeof(struct stru_4) == 8
```
```
struct ex_4{
  uint8_t a
  uint8_t b
  uint16_t c
  uint32_t d
}
sizeof(struct ex_3) == 12
```
