# build options
include ../common.mk
include ./block.mk
	
%.bin: %.elf
	@${OBJCOPY} -O binary $< $@
	
%_objdump.txt: ${TASK_ELFS}
	@${OBJDUMP} --disassemble-all $^ > $@ 
	
${TASK_ELFS}: %.elf: ${LIB_OBJS} ${TASK_OBJS}
	${CC} ${CFLAGS} -T block.ld -Wl,--no-warn-rwx-segments -o $@ $^ -lc

%.o : %.c 
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"

%.o : %.S task-compile-console
	@${CC} ${CFLAGS} -c -o $@ $<
	@echo "  $< ---> $@"

task-compile-console:
	@echo "  ============ COMPILE TASKS ==========="
	@echo ""

# execute each binary file with python script once and generate a c file respectively
all: ${TASK_BINS} ${TASK_DISASMS}
	@echo ""
	@echo "  ============ GENERATE BINS ==========="
	@echo ""
	@python python/read_bin.py ${TASK_BINS}
	@python python/modify_linker.py
	@python python/modify_mem.py
	@python python/modify_header.py

.PHONY: clean
clean:
	@cd lib && rm -rf *.o
	@cd code && rm -rf *.o
	@cd cbin && rm -rf *
	@cd build && rm -rf *
	rm -rf *.elf *.bin *_bin.c *.txt
	@python python/rollback_linker.py
	@python python/rollback_mem.py
	@python python/rollback_header.py


