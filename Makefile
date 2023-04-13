include ./common.mk

all: os.elf

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)

# start.o must be the first in dependency!
os.elf: ${OBJS}
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

.PHONY : venus
venus: os.elf	
	@${OBJCOPY} -O binary os.elf os.bin
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@hexdump os.bin > bin.txt
	


.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out *.map



