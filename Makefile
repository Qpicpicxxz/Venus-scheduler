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

.PHONY : code
code: all
	@echo ""
	@echo "==============================================="
	@echo "Compiling all the object files above together"
	@echo "TOBE an ELF file and disassemble into objdump.txt"
	@echo "================================================"
	@${OBJCOPY} -O binary os.elf os.bin
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@hexdump os.bin > bin.txt

.PHONY : symbol
symbol: all
	@${NM} -n os.elf
	
.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out  *.map
	@cd src && rm -rf *.o *.bin *.elf *.txt



