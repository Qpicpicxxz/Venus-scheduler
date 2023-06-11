include ./common.mk
include ./scheduler.mk

all: venus

# start.o must be the first in dependency!
os.elf: ${OBJS}
	@${CC} ${CFLAGS} -T os.ld -Wl,--no-warn-rwx-segments -o os.elf ${OBJS}  $< -lc

${OBJS}: task-compile

%.o : %.c 
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"

%.o : %.S
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"


.PHNOY: task-compile
task-compile:
	@cd ${TASK} && ${MAKE} all
	@echo ""
	@echo "  ========== COMPILE SCHEDULER ========="
	@echo ""

.PHONY : venus
venus: os.elf	
	@${OBJCOPY} -O binary os.elf os.bin
	@${OBJDUMP} --disassemble-all os.elf > objdump.txt
	@hexdump os.bin > bin.txt
	
.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf *.txt *.out *.map *.zip *_bin.c
	@cd src && rm -rf *.o *.bin *.elf *.txt *_bin.c
	@cd src/dma && rm -rf *.o *.bin *.elf *.txt
	@cd ${TASK} && ${MAKE} clean



