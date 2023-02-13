CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -march=rv32ima -mabi=ilp32 -g -Wall
QEMU = qemu-system-riscv32
QFLAGS = -nographic -machine virt -bios none \
				 -smp cores=8,threads=1,sockets=1 \
				 -object memory-backend-ram,size=1M,id=m0 \
				 -object memory-backend-ram,size=127M,id=m1 \
				 -numa node,cpus=0,nodeid=0,memdev=m0 \
				 -numa node,cpus=1-7,nodeid=1,memdev=m1

GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm

SRCS_ASM = \
					 start.S \
					 mem.S


SRCS_C = \
				 kernel.c \
			 	 uart.c \
				 printf.c \
				 debug.c \
				 user.c

# the source file we want to compile separately
SRCS_TASKS = \
				task1.c \
				task2.c \
				task3.c

SRCS_BIN = $(patsubst %.c,%_bin.c,$(SRCS_TASKS))
				
.DEFAULT_GOAL := all
