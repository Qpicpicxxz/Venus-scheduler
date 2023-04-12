CROSS_COMPILE = riscv32-unknown-elf-
CFLAGS = -I./include -nostdlib -fno-builtin -ffreestanding -march=rv32ima -mabi=ilp32 -g -Wall -Wl,-Map=output.map
QEMU = qemu-system-riscv32
QFLAGS = -nographic -machine virt -bios none \
				 -smp cores=1,threads=1,sockets=1

SOURCE = src/
GDB = gdb-multiarch
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
NM = ${CROSS_COMPILE}nm
MAKEFLAGS += --silent

