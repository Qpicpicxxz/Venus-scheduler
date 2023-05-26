# Venus-scheduler

### This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband

##### Task dependency depict example

<div align="center">
  <img src="https://github.com/Qpicpicxxz/Venus-scheduler/tree/main/image/DAG.png">
</div>

```
void dag_create(void) {
      /* Create every actor's description in dependency graph */
      actor_t* A = actor_create(TASKA_START, TASKA_LEN);
      actor_t* B = actor_create(TASKB_START, TASKB_LEN);
      actor_t* C = actor_create(TASKC_START, TASKC_LEN);
      actor_t* D = actor_create(TASKD_START, TASKD_LEN);
      actor_t* E = actor_create(TASKE_START, TASKE_LEN);
      /* Assign dependencies between actors  */
      edge_make(A, 0, B, 0);
      edge_make(A, 3, B, 1);
      edge_make(A, 0, C, 0);
      edge_make(A, 1, C, 1);
      edge_make(A, 2, C, 2);
      edge_make(A, 3, D, 0);
      edge_make(B, 0, E, 0);
      edge_make(C, 0, E, 1);
      edge_make(D, 0, E, 2);
      /* Specify root actor and tail actor */
      assign_root(A);
      assign_sink(E);
      /* Input stimulate data packet in root actor */
      packet_input(A, (uint32_t)data_packet_addr, data_packet_length)；
}
```

```
.
├── common.mk           // rules and files to compile scheduler code
├── dma
│   ├── chx_cfg.c       // CHx_CFG register concatenate
│   ├── chx_ctl.c       // CHx_CTL register concatenate
│   ├── dma.c           // scheduler & dma interface
│   ├── lli_create.c    // setup DMA linked list item
│   ├── phy_interface   // handlers to write/read physical address
│   └── transfer_callback.c  // a callback to handle DMA transfer done
├── gdbinit             // initialize GNU debuger script
├── include             // header files
│   ├── actor.h         // define actor struct
│   ├── allocator.h     // memAlloc && memBlock header file
│   ├── assert.h        // assert macro
│   ├── block.h         // define Block-task relation flags
│   ├── codeaddr.h      // declare task code's position
│   ├── common.h        // basic tool functions
│   ├── daddr.h         // define VENUS block's data memory address
│   ├── defs.h          // declare some task relative structs
│   ├── dma
│   │   ├── cfg.h
│   │   ├── common.h    // common functions relative to DMA
│   │   ├── ctl.h       // CHx_CTL configurations
│   │   ├── lli.h       // define DMA linked list item structure
│   │   ├── msg.h       // relative information of channel, lli and data
│   │   ├── offset.h
│   │   └── phy.h
│   ├── fifo.h          // define data && fifo && queue structure type
│   ├── font.h          // font style for printf
│   ├── hw              // venus block address map definitions
│   │   ├── addressmap.h
│   │   ├── axi.h       // axi bus addressmap
│   │   ├── blockcsr.h  // block's csr addressmap
│   │   ├── config.h    // define required cluster and block
│   │   └── uart.h
│   ├── linkedlist.h    // header file of linked list
│   ├── platform.h      // define machine's physical property
│   ├── README.md       // introduce all the struct descriptor
│   ├── riscv.h         // some basic privileged register assemble instructions
│   ├── saddr.h         // VENUS block's scratchpad memory address
│   ├── task.h          // include "actor.h && block.h" and extern some functions
│   ├── types.h         // unifom bit length
│   └── ulib.h          // memory and string handlers
├── Makefile
├── os.ld               // linker script
├── README.md           // introduction
├── src
│   ├── actor_create.c  // API export to user to depict DAG
│   ├── actor_init.c    // depict DAG and declare actors and fifo-queues
│   ├── block.c         // simulate block compute behaviour
│   ├── dag.c           // python script generated DAG
│   ├── debug.c         // print out all sections range and size
│   ├── dma.c           // simulate DMA's behaviour
│   ├── external_irq.c  // initialize block's struct and handle external interrupt
│   ├── fifo.c          // some method to operate FIFO
│   ├── fire_check.c    // handler to check whether the task could fire
│   ├── kernel.c        // initialize all the scheduler functions
│   ├── linked_list.c   // handlers to manipulate a link list
│   ├── main.c          // main function to run scheduler and launch actors
│   ├── mem_alloc.c     // malloc & free function
│   ├── mem_block.c     // some functions to control allocation unit blocks
│   ├── mem_free.c      // handlers to manage free block list
│   ├── mem.S           // define some macros for catch section's address
│   ├── printf.c        // print strings to QEMU terminal
│   ├── README.md       // simple struct of current mechanism
│   ├── start.S         // first code to execute when launch a kernel
│   ├── switch.S        // some method switch operation
│   ├── task_callback.c // callback operation after block pull the interrupt
│   ├── trap.c          // catch interrupt and exception
│   ├── uart.c          // UART functions for print && external interrupt
│   └── ulib.c          // memory and string c lib functions
└── task
    ├── generate_dag.py  // create a DAG randomly
    ├── Makefile
    ├── modify_header.py
    ├── modify_linker.py // auto add tasks section into os.ld
    ├── modify_mem.py    // auto define tasks descriptor into src/mem.S
    ├── read_bin.py      // python script to rewrite binary file to c file
    ├── rollback_header.py
    ├── rollback_link.py // rollback the changes to the os.ld
    ├── rollback_mem.py  // rollback the changes to the src/mem.S
    ├── task1.c          // tasks we want load to other blocks
    ├── task2.c
    ├── task3.c
    └── task.ld          // linker script to control task's
5 directories, 75 files
```
