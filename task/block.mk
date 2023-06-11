# To make initialized zero global variables are compiled into .data section
CFLAGS += -fno-zero-initialized-in-bss
SOURCE = lib
TASKCODE = code
PYTHON = python
BUILD = build

SRCS_ASM = \
		${SOURCE}/start.S \
		${SOURCE}/mem.S

SRCS_C = \
		${SOURCE}/printf.c \
		${SOURCE}/uart.c \
		${SOURCE}/epilogue.c \
		${SOURCE}/init.c \
		${SOURCE}/ulib.c \
		${SOURCE}/mem_alloc.c \
		${SOURCE}/mem_block.c \
		${SOURCE}/mem_free.c \



