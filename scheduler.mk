SOURCE = src
DMA = src/dma
TASK = task
TASKCODE = task/code
TASKBIN = task/cbin
CFLAGS += -O3

SRCS_ASM = \
		$(SOURCE)/irq_vector.S \
		$(SOURCE)/start.S \
		$(SOURCE)/hal.S \
		$(SOURCE)/mem.S

SRCS_C = \
		$(SOURCE)/kernel.c \
		$(SOURCE)/uart.c \
		$(SOURCE)/init.c \
		$(SOURCE)/printf.c \
		$(SOURCE)/ulib.c \
		$(SOURCE)/irq.c \
		$(SOURCE)/mem_alloc.c \
		$(SOURCE)/linked_list.c \
		$(SOURCE)/actor_init.c \
		$(SOURCE)/dag_callback.c \
		$(SOURCE)/padding.c \
		$(DMA)/dma_irq.c \
		$(DMA)/dma_lli_create.c \
		$(DMA)/dma_transfer_done.c 

SRCS_BIN = $(patsubst $(TASKCODE)/%.c, $(TASKBIN)/%_bin.c, $(wildcard $(TASKCODE)/*.c))

OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)
OBJS += $(SRCS_BIN:.c=.o)

