# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband
```
.
├── common.mk           // rules and files to compile scheduler code
├── gdbinit             // initialize GNU debuger script
├── include             // header files
│   ├── actor.h         // define data && fifo && actor structure type
│   ├── block.h         // define Block-task relation flags
│   ├── daddr.h         // define VENUS block's data memory address
│   ├── os.h            // include all the header file and define tasks context
│   ├── platform.h      // define machine's physical property
│   ├── riscv.h         // some basic privileged register assemble instructions
│   ├── saddr.h         // VENUS block's sratchpad memory address
│   ├── tasks.h         // include "actor.h && block.h" and extern some functions
│   └── types.h         // unifom bit length
├── Makefile
├── os.ld               // linker script
├── README.md           // introduction
├── src
│   ├── actorInit.c     // depict DAG and declare actors and fifo-queues
│   ├── block.c         // simulate block compute behaviour
│   ├── debug.c         // print out all sections range and size
│   ├── dma.c           // simulate DMA's behaviour
│   ├── externalIRQ.c   // initialize block's struct and handle external interrupt
│   ├── fifo.c          // some method to operate FIFO
│   ├── kernel.c        // initialize all the scheduler functions
│   ├── main.c          // main function to run scheduler and launch actors
│   ├── mem.S           // define some macros for catch section's address
│   ├── page.c          // dynamic allocation for page (4KiB)
│   ├── printf.c        // print strings to QEMU terminal
│   ├── sctx.c          // some method for switch operation
│   ├── start.S         // first code to execute when launch a kernel
│   ├── switch.S        // some method switch operation
│   ├── taskrule.c      // define block_recycle() && task_bind() && task()
│   ├── trap.c          // catch interrupt and exception
│   └── uart.c          // UART functions for print && external interrupt
└── task
    ├── Makefile
    ├── readbin.py      // python script to rewrite binary file to c file
    ├── task1.c         // tasks we want load to other blocks
    ├── task2.c
    ├── task3.c
    └── task.ld         // linker script to control task's
3 directories, 37 files
```

