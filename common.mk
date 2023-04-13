CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -I./include -nostdlib -fno-builtin -ffreestanding -march=rv32ima -mabi=ilp32 -g -Wall -Wl,-Map=output.map -O0

SOURCE = src/
DMA = src/dma
GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm
MAKEFLAGS += --silent

SRCS_ASM = \
		$(SOURCE)start.S 

SRCS_C = \
		$(SOURCE)kernel.c \
		$(SOURCE)uart.c \
		$(SOURCE)devctrl.c \
		$(SOURCE)printf.c \
		$(SOURCE)ulib.c
		
