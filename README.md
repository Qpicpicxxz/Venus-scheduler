# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband
```
.
├── common.mk           // rules and files to compile scheduler code
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
│   ├── fifo.h          // define data && fifo && queue structure type
│   ├── font.h          // font style for printf
│   ├── linkedlist.h    // header file of linked list
│   ├── platform.h      // define machine's physical property
│   ├── README.md	// introduce all the struct descriptor
│   ├── riscv.h         // some basic privileged register assemble instructions
│   ├── saddr.h         // VENUS block's scratchpad memory address
│   ├── task.h          // include "actor.h && block.h" and extern some functions
│   ├── types.h         // unifom bit length
│   └── ulib.h          // memory and string handlers
├── Makefile
├── os.ld               // linker script
├── README.md           // introduction
├── src
│   ├── actor_create.c	// API export to user to depict DAG
│   ├── actor_init.c     // depict DAG and declare actors and fifo-queues
│   ├── block.c         // simulate block compute behaviour
│   ├── dag.c           // python script generated DAG
│   ├── debug.c         // print out all sections range and size
│   ├── dma.c           // simulate DMA's behaviour
│   ├── external_irq.c   // initialize block's struct and handle external interrupt
│   ├── fifo.c          // some method to operate FIFO
│   ├── fire_check.c     // handler to check whether the task could fire
│   ├── kernel.c        // initialize all the scheduler functions
│   ├── linked_list.c    // handlers to manipulate a link list
│   ├── main.c          // main function to run scheduler and launch actors
│   ├── mem_alloc.c      // malloc & free function
│   ├── mem_block.c      // some functions to control allocation unit blocks
│   ├── mem_free.c       // handlers to manage free block list
│   ├── mem.S           // define some macros for catch section's address
│   ├── printf.c        // print strings to QEMU terminal
│   ├── README.md       // simple struct of current mechanism
│   ├── start.S         // first code to execute when launch a kernel
│   ├── switch_context.c // some method for switch operation
│   ├── switch.S        // some method switch operation
│   ├── task_callback.c  // callback operation after block pull the interrupt
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
    ├── task1.c         // tasks we want load to other blocks
    ├── task2.c
    ├── task3.c
    └── task.ld         // linker script to control task's
3 directories, 61 files
```

