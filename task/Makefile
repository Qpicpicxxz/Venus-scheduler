# build options
include ../common.mk

# task.elf and task.bin
ELF = $(patsubst %.c,%.elf,$(SRCS_TASKS))
BINARY = $(patsubst %.c,%.bin,$(SRCS_TASKS))

# label
.PHONY: word
word:
	@echo ""
	@echo "============ COMPILING ============="
	@echo ""

elf: ${ELF}

# in this step will output linker script(ld) warning: task1.elf has a LOAD segment with RWX permissions
%.elf : %.c
	@${CC} ${CFLAGS} -T task.ld -o $@ $< 2>/dev/null
	@echo "  $< ---> $@"

%.bin : %.elf
	@${OBJCOPY} -O binary $< $@
	@echo "  $< ---> $@"

# execute each binary file with python script once and generate a c file respectively
all: word ${BINARY}
	@echo ""
	@echo "============ CONVERTING ============"
	@echo ""
	@python readBin.py ${BINARY}
	@echo ""
	@echo "============ MOVING CODE ==========="
	@echo ""
	@cp *_bin.c ../
	@echo "  all the generated files have been"
	@echo "  copied to the root directory"
	@echo ""
	@echo "========== MODIFYING LINKER ========"
	@python modifyLinker.py
	@echo ""
	@echo "  place every task codes' section"
	@echo ""
	@echo "========== MODIFYING mem.S ========="
	@python modifyMem.py
	@echo ""
	@echo "  define every task codes' description"
	
.PHONY: code
code: all
	@echo ""
	@echo "=========== DISASSEMBLING ========="
	@echo ""
	@${OBJDUMP} --disassemble-all task1.elf > objdump.txt
	@open objdump.txt

.PHONY: clean
clean:
	rm -rf *.elf *.bin *_bin.c *.txt
	@python rollbackLink.py
	@python rollbackMem.py

