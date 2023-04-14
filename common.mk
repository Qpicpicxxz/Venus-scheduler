CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -I./include -nostdlib -fno-builtin -ffreestanding -march=rv32im -mabi=ilp32 -g -Wall -Wl,-Map=output.map

SOURCE = src/
DMA = src/dma/
GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm
MAKEFLAGS += --silent

SRCS_ASM = \
		$(SOURCE)irq_vector.S \
		$(SOURCE)start.S \
		$(SOURCE)hal.S \
		$(SOURCE)mem.S

SRCS_C = \
		$(SOURCE)kernel.c \
		$(SOURCE)uart.c \
		$(SOURCE)devctrl.c \
		$(SOURCE)printf.c \
		$(SOURCE)ulib.c \
		$(SOURCE)irq.c \
		$(SOURCE)mem_alloc.c \
		$(SOURCE)mem_block.c \
		$(SOURCE)mem_free.c \
		$(DMA)dma_config.c \
		$(DMA)dma_irq.c \
		$(DMA)dma_lli_create.c 
		
