CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -ffreestanding -march=rv32ima -mabi=ilp32 -g -Wall
QEMU = qemu-system-riscv32
QFLAGS = -nographic -machine virt -bios none \
				 -smp cores=1,threads=1,sockets=1

GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm

SRCS_ASM = \
					 start.S \
					 mem.S \
					 entry.S


SRCS_C = \
				 kernel.c \
			 	 uart.c \
				 printf.c \
				 debug.c \
				 sched.c \
				 fifo.c \
				 scheduler.c \
				 dma.c \
				 taskrule.c

# the source file we want to compile separately
SRCS_TASKS = \
				task1.c \
				task2.c \
				task3.c

SRCS_BIN = $(patsubst %.c,%_bin.c,$(SRCS_TASKS))

.DEFAULT_GOAL := all
