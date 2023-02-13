# Venus-scheduler
###  This is a scheduler for VENUS: A Wireless Multi-core Processing Unit for 5G baseband
```
├── common.mk   // rules and files to compile scheduler code and QEMU
├── debug.c     // print out all sections range and size
├── gdbinit     // initialize GNU debuger script
├── kernel.c    // initialize all the scheduler functions
├── Makefile    // compile option
├── mem.S       // define some macros for catch section's address
├── os.h        // include all the header file and define tasks context
├── os.ld       // linker script
├── platform.h  // define machine's physical property
├── printf.c    // print strings to QEMU terminal
├── README.md   // introduction
├── start.S     // first code to execute when launch a kernel
├── task        
│   ├── Makefile
│   ├── readbin.py    // python script to rewrite binary file to c file
│   ├── task1.c       // tasks we want load to other blocks
│   ├── task2.c
│   └── task3.c
├── types.h     // define the standard word length 
├── uart.c      // UART functions for print
└── user.c        
```
