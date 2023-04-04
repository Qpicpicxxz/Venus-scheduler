CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -I./include -nostdlib -fno-builtin -ffreestanding -march=rv32ima -mabi=ilp32 -g -Wall -Wl,-Map=output.map
QEMU = qemu-system-riscv32
QFLAGS = -nographic -machine virt -bios none \
				 -smp cores=1,threads=1,sockets=1

SOURCE = src/
DMA = dma/
GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm
MAKEFLAGS += --silent
CFLAGS += -DDEBUG_SCHEDULER
CFLAGS += -DSIMULATE_QEMU

SRCS_ASM = \
					 $(SOURCE)start.S \
					 $(SOURCE)mem.S \
					 $(SOURCE)switch.S


SRCS_C = \
				 $(SOURCE)kernel.c \
			 	 $(SOURCE)uart.c \
				 $(SOURCE)printf.c \
				 $(SOURCE)ulib.c  \
				 $(SOURCE)mem_block.c \
				 $(SOURCE)mem_alloc.c \
				 $(SOURCE)mem_free.c \
				 $(SOURCE)linked_list.c \
				 $(SOURCE)switch_context.c \
				 $(SOURCE)trap.c \
				 $(SOURCE)fifo.c \
				 $(SOURCE)debug.c \
				 $(SOURCE)main.c \
				 $(SOURCE)block.c \
				 $(SOURCE)task_callback.c \
				 $(SOURCE)external_irq.c \
				 $(SOURCE)actor_init.c \
				 $(SOURCE)fire_check.c	\
				 $(SOURCE)actor_create.c \
				 $(SOURCE)dag.c \
				 $(DMA)lli_create.c \
				 $(DMA)dma.c    \
				 $(DMA)chx_ctl.c \
				 $(DMA)chx_cfg.c \
				 $(DMA)phy_interface.c \
				 $(DMA)transfer_callback.c
				 
SRCS_BIN = $(patsubst %.c,%_bin.c,$(SRCS_TASKS))

SRCS_TASKS :=\
		task1.c \
		task2.c \
		task3.c 

.DEFAULT_GOAL := all
