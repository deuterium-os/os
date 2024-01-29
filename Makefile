TARGET := x86_64-elf

CROSS_BIN_PATH := ~/opt/cross/bin
CC := $(CROSS_BIN_PATH)/$(TARGET)-gcc
AR := $(CROSS_BIN_PATH)/$(TARGET)-ar
AS := $(CROSS_BIN_PATH)/$(TARGET)-as
LD := $(CROSS_BIN_PATH)/$(TARGET)-ld
OBJCOPY := $(CROSS_BIN_PATH)/$(TARGET)-objcopy
NASM := nasm
MKBOOTIMG := mkbootimg
QEMU := qemu-system-x86_64

TARGET_ARCH := x86
ARCHDIR := arch/$(TARGET_ARCH)

CFILES := $(shell find -L * -type f -name '*.c')
ASFILES := $(shell find -L * -type f -name '*.S')
NASMFILES := $(shell find -L * -type f -name '*.asm')

OBJS := $(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o) kernel/font.o

DEPS := $(CFILES:.c=.c.d)
-include $(DEPS)

CFLAGS ?= -g
CPPFLAGS ?=
LDFLAGS ?=
NASMFLAGS ?=

CFLAGS :=\
	$(CFLAGS) \
	-Wall \
	-Wextra\
	-std=gnu11 \
	-ffreestanding \
	-fno-stack-protector \
	-fPIC \
	-march=x86-64 \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \

CPPFLAGS :=\
	-I include \
	$(CPPFLAGS) \
	-MMD \
	-MP

LDFLAGS :=\
	$(LDFLAGS) \
	-nostdlib \
	-n \
	-T $(ARCHDIR)/linker.ld \
	-lgcc

NASMFLAGS :=\
	-f elf64

all: kernel.bin

kernel.bin: $(ARCHDIR)/linker.ld $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.asm.o: %.asm
	$(NASM) $(NASMFLAGS) -o $@ $<

%.o: %.psf
	# Trick to let linker generates correct symbol
	cd $(dir $<) && $(OBJCOPY) -O elf64-x86-64 -B i386 -I binary $(notdir $<) $(notdir $@) && cd -

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f kernel.bin
	rm -f deuterium-os.img
	rm -rf imgdir

img: kernel.bin
	mkdir -p imgdir/boot
	cp kernel.bin imgdir/boot/
	$(MKBOOTIMG) mkbootimg.json deuterium-os.img

run: img
	qemu-system-x86_64 \
	-drive file=deuterium-os.img,media=disk,format=raw \
	-gdb tcp::1234 \
	-S

.PHONY: all clean img