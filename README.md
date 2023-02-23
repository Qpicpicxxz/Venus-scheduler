# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband
```
├── actor.h     // define some fifo && dma && tasks attributes
├── common.mk   // rules and files to compile scheduler code and QEMU
├── daddr.h     // define VENUS block's data memory address
├── debug.c     // print out all sections range and size
├── dma.c       // simulate DMA and VENUS block's behaviour
├── fifo.c      // some method to operate FIFO
├── gdbinit     // initialize GNU debuger script
├── kernel.c    // initialize all the scheduler functions
├── main.c      // main function to fire dataflow tasks
├── Makefile    // compile option
├── mem.S       // define some macros for catch section's address
├── os.h        // include all the header file and define tasks context
├── os.ld       // linker script
├── page.c      // dynamic allocation for memory
├── page.h      // header file for malloc
├── platform.h  // define machine's physical property
├── printf.c    // print strings to QEMU terminal
├── README.md   // introduction
├── riscv.h     // some basic privileged register assemble instructions
├── saddr.h     // VENUS block's sratchpad memory address
├── sctx.c      // some method for switch operation
├── start.S     // first code to execute when launch a kernel
├── switch.S    // some method switch operation
├── task
│   ├── Makefile
│   ├── readbin.py    // python script to rewrite binary file to c file
│   ├── task1.c       // tasks we want load to other blocks
│   ├── task2.c
│   ├── task3.c
│   └── task.ld       // linker script to control task's compilation
├── taskrule.c  // define the rules on how the actor fires upon every invocation
├── types.h     // define the standard word length
└── uart.c      // UART functions for print
```

