include ./common.mk

all: os.elf

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)
OBJS += $(patsubst %.c,%.o,$(SRCS_BIN))

# start.o must be the first in dependency!
os.elf: word ${OBJS}
	@echo "  *.o os.ld ---> os.elf"
	@${CC} ${CFLAGS} -T os.ld -Wl,--no-warn-rwx-segments -o os.elf ${OBJS} -lc

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
	@echo "============ COMPILING ============="
	@echo ""

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
	@echo "TOBE an ELF file and disassemble into objdump.txt"
	@echo "================================================"
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@open objdump.txt

.PHONY : symbol
symbol: all
	@${NM} -n os.elf
	
	
.PHONY : bin
bin: all
	@echo ""
	@echo "==============================================="
	@echo "  Objcopy .elf file to binary files .bin"
	@echo "==============================================="
	@${OBJCOPY} -O binary os.elf os.bin
	@hexdump os.bin > bin.txt
	@open bin.txt 

.PHONY : tree
tree:
	tree > tree.txt
	@open tree.txt

.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out  *.map
	@cd src && rm -rf *.o *.bin *.elf *.txt
	@cd task && ${MAKE} clean



