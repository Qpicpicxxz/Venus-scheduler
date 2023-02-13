include ./common.mk

all: os.elf

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)
OBJS += $(patsubst %.c,%.o,$(SRCS_BIN))

# start.o must be the first in dependency!
os.elf: taskgenerate word ${OBJS}
	@${CC} ${CFLAGS} -T os.ld -Wl,--no-warn-rwx-segments -o os.elf ${OBJS}

$(patsubst %.c,%.o,$(SRCS_BIN)): %.o : %.c 
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"

%.o : %.c 
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"

%.o : %.S
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"

word:
	@echo ""
	@echo "============ COMPILING ============"
	@echo ""

taskgenerate:
	@cd task && ${MAKE} all

# start qemu the virtual machine and run the code
.PHONY: run
run: all
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo ""
	@echo "========== QEMU LAUNCHED ==========="
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "===================================="
	@echo ""
	@${QEMU} ${QFLAGS} -kernel os.elf

.PHONY : dbg
dbg: all
	@echo ""
	@echo "Press Ctrl-C and then input 'q' to exit GDB and QEMU"
	@echo "===================================================="
	@echo ""
	@${QEMU} ${QFLAGS} -kernel os.elf -s -S &
	@${GDB} os.elf -q -x ./gdbinit

.PHONY : code
code: all
	@echo ""
	@echo "==============================================="
	@echo "Compiling all the object files above together"
	@echo "TOBE an ELF file and disassemble into a textfile"
	@echo "================================================"
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@open objdump.txt

.PHONY : symbol
symbol: all
	@${NM} -n os.elf

.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out *_bin.c
	@cd task && ${MAKE} clean



