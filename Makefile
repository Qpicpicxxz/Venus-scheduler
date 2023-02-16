include ./common.mk


all: libtask.a os.elf

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)

libtask.a: 
	@${CC} ${CFLAGS} -c -o task1.o task1.c
	@${CC} ${CFLAGS} -c -o task2.o task2.c
	@${CC} ${CFLAGS} -c -o task3.o task3.c
	@${AR} -crv libtask.a task1.o task2.o task3.o > /dev/null
	@echo "================== ARCHIVING ================="
	@echo "  All the task have been archived to libtask.a"
	@echo "=============================================="
	@echo ""

# start.o must be the first in dependency!
os.elf: ${OBJS}
	@${CC} ${CFLAGS} -T os.ld -Wl,--no-warn-rwx-segments,--whole-archive,-Map=output.map -o os.elf ${OBJS} libtask.a

%.o : %.c 
	@${CC} ${CFLAGS} -L. -ltask -c -o $@ $<
	@echo "  $< ---> $@"

%.o : %.S
	@${CC} ${CFLAGS} -L. -ltask -c -o $@ $<
	@echo "  $< ---> $@" 

# start qemu the virtual machine and run the code
.PHONY: run
run: all
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo ""
	@echo "=========== QEMU LAUNCHED ============"
	@echo "  Press Ctrl-A and then X to exit QEMU"
	@echo "======================================"
	@echo ""
	@${QEMU} ${QFLAGS} -kernel os.elf

.PHONY : dbg
dbg: all
	@echo ""
	@echo "  Press Ctrl-C and then input 'q' to exit GDB and QEMU"
	@echo "======================================================"
	@echo ""
	@${QEMU} ${QFLAGS} -kernel os.elf -s -S &
	@${GDB} os.elf -q -x ./gdbinit

.PHONY : code
code: all
	@echo ""
	@echo "=================================================="
	@echo "  Compiling all the object files above together"
	@echo "  TOBE an ELF file and disassemble into a textfile"
	@echo "=================================================="
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@open objdump.txt

.PHONY : symbol
symbol: all
	@${NM} -n os.elf

.PHONY : static
static: libtask.a os.elf
	@${OBJDUMP} --disassemble-all os.elf > static.txt
	@${OBJDUMP} -d libtask.a > lib.txt
	@open static.txt
	@open lib.txt
	

.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out *_bin.c *.a
	@cd task && ${MAKE} clean



