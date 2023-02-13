# build options
include ../common.mk

# task.elf and task.bin
ELF = $(patsubst %.c,%.elf,$(SRCS_TASKS))
BINARY = $(patsubst %.c,%.bin,$(SRCS_TASKS))

# label
.PHONY: word
word:
	@echo ""
	@echo "============ COMPILING ============"
	@echo ""

elf: ${ELF}

%.elf : %.c
	@${CC} ${CFLAGS} -o $@ $<
	@echo "  $< ---> $@"

%.bin : %.elf
	@${OBJCOPY} -O binary $< $@
	@echo "  $< ---> $@"

# execute each binary file with python script once and generate a c file respectively
all: word ${BINARY}
	@echo ""
	@echo "============ CONVERTING ============"
	@echo ""
	@python readbin.py ${BINARY}
	@echo ""
	@echo "============ MOVING CODE ==========="
	@echo ""
	@cp *_bin.c ../
	@echo "  all the generated files have been copied to the root directory"
	@echo ""

.PHONY: clean
clean:
	rm -rf *.elf *.bin *_bin.c

