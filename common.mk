CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -I./include -nostdlib -fno-builtin -ffreestanding -march=rv32ima -mabi=ilp32 -g -Wall
QEMU = qemu-system-riscv32
QFLAGS = -nographic -machine virt -bios none \
				 -smp cores=1,threads=1,sockets=1

SOURCE = src/
GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm

SRCS_ASM = \
					 $(SOURCE)start.S \
					 $(SOURCE)mem.S \
					 $(SOURCE)switch.S


SRCS_C = \
				 $(SOURCE)kernel.c \
			 	 $(SOURCE)uart.c \
				 $(SOURCE)printf.c \
				 $(SOURCE)debug.c \
				 $(SOURCE)page.c \
				 $(SOURCE)switchContext.c \
				 $(SOURCE)trap.c \
				 $(SOURCE)fifo.c \
				 $(SOURCE)main.c \
				 $(SOURCE)dma.c \
				 $(SOURCE)taskCallback.c \
				 $(SOURCE)block.c \
				 $(SOURCE)externalIRQ.c \
				 $(SOURCE)actorInit.c \
				 $(SOURCE)linkedList.c \
				 $(SOURCE)fireCheck.c

# the source file we want to compile separately
SRCS_TASKS = \
				task1.c \
				task2.c \
				task3.c

SRCS_BIN = $(patsubst %.c,%_bin.c,$(SRCS_TASKS))

.DEFAULT_GOAL := all
