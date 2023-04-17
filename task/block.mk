SOURCE = lib
TASKCODE = code
PYTHON = python
BUILD = build

SRCS_ASM = \
		${SOURCE}/start.S

SRCS_C = \
		${SOURCE}/printf.c \
		${SOURCE}/uart.c 

SRCS_TASK = $(wildcard ${TASKCODE}/*.c)

LIB_OBJS = $(SRCS_ASM:.S=.o)
LIB_OBJS += $(SRCS_C:.c=.o)
TASK_OBJS = $(SRCS_TASK:.c=.o)

TASK_ELFS = $(addprefix $(BUILD)/,$(foreach src,$(SRCS_TASK),$(basename $(notdir ${src})).elf))
TASK_DISASMS = $(addprefix $(BUILD)/,$(notdir $(TASK_ELFS:.elf=_objdump.txt)))
TASK_BINS = $(addprefix $(BUILD)/,$(notdir $(TASK_ELFS:.elf=.bin)))

