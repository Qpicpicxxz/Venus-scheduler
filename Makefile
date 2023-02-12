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
				 user.c \
				 task1.c \
				 task2.c 

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)

.DEFAULT_GOAL := all
all: os.elf

# start.o must be the first in dependency!
os.elf: ${OBJS}
	@${CC} ${CFLAGS} -T os.ld -Wl,--no-warn-rwx-segments -o os.elf $^

%.o : %.c
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "$< ---> $@"

%.o : %.S
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "$< ---> $@"

# start qemu the virtual machine and run the code
.PHONY: run
run: all
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "------------------------------------"
	@${QEMU} ${QFLAGS} -kernel os.elf

# single step debug
.PHONY : dbg
dbg: all
	@echo "Press Ctrl-C and then input 'q' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@${QEMU} ${QFLAGS} -kernel os.elf -s -S &
	@${GDB} os.elf -q -x ./gdbinit

.PHONY : code
code: all
	@echo "-------------------------------------------------"
	@echo "Compiling all the object files above together"
	@echo "TOBE an ELF file and disassemble into a textfile"
	@echo "-------------------------------------------------"
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@open objdump.txt

.PHONY : singlecompile
singlecompile: test.c
	${CC} ${CFLAGS} -o test.elf test.c
	${OBJDUMP} --disassemble-all test.elf	> test.txt
	@${OBJCOPY} -O binary test.elf test.bin
	@open test.txt

symbol: all
	@${NM} -n os.elf

.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out


