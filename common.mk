CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -march=rv32ima -mabi=ilp32 -g -Wall
QEMU = qemu-system-riscv32
QFLAGS = -nographic -machine virt -bios none \
			#	 -smp cores=8 \
				 -numa node,nodeid=0,cpus=0,memdev=1G \
				 -numa node,nodeid=1,cpus=4-7,memdev=1G 

GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm
AR = ${CROSS_COMPILE}ar

SRCS_ASM = \
					 start.S \
					 mem.S


SRCS_C = \
				 kernel.c \
			 	 uart.c \
				 printf.c \
				 debug.c \
#				 scheduler.c

# the source file we want to compile separately
SRCS_TASKS = \
				task1.c \
				task2.c \
				task3.c 

SRCS_BIN = $(patsubst %.c,%_bin.c,$(SRCS_TASKS))
				
.DEFAULT_GOAL := all
