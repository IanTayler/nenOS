TOOLPREFIX		= i686-elf-
ARCH			= x86
OPT				= -O2

CC				= $(TOOLPREFIX)gcc
LD				= $(CC)
AS				= $(TOOLPREFIX)as

OUTPUTPREF		= ./iso/boot/
OUTPUTNAME 		= $(OUTPUTPREF)nenos.bin

BUILDPATH		= ./build/

SOURCEPREF		= ./src/
INCLUDEPREF		= $(SOURCEPREF)include/
ASMPREFIX		= ./src/arch/$(ARCH)/

# Inner C flags.
ICFLAGS 		= -I$(SOURCEPREF) -I$(INCLUDEPREF)\
				  -I$(ASMPREFIX) -c
# Outer C flags.
OCFLAGS 		= -ffreestanding $(OPT) -Wall -Wextra

ILDFLAGS 		= -T $(LINKSCRIPT) -ffreestanding $(OPT) -nostdlib
OLDFLAGS 		= -o $(OUTPUTNAME) -lgcc

INCLUSIONS		= $(INCLUDEPREF)system.h $(INCLUDEPREF)head.h
SOURCES			= $(SOURCEPREF)interface.c

KERNELPATH		= $(SOURCEPREF)kernel.c

LINKSCRIPT		= $(ASMPREFIX)link.ld

.PHONY: all test iso

all: $(ASMPREFIX)boot.s $(BUILDPATH)boot.o $(BUILDPATH)kernel.o
	$(LD) $(ILDFLAGS) $(BUILDPATH)boot.o $(BUILDPATH)kernel.o \
	$(BUILDPATH)interface.o $(BUILDPATH)vga.o $(BUILDPATH)gdt.o \
	$(OLDFLAGS)

$(BUILDPATH)boot.o: $(ASMPREFIX)boot.s
	$(AS) -c $(ASMPREFIX)boot.s -o $(BUILDPATH)boot.o

$(BUILDPATH)kernel.o: $(KERNELPATH) $(INCLUSIONS)\
					  $(BUILDPATH)interface.o\
					  $(BUILDPATH)vga.o\
					  $(BUILDPATH)gdt.o
	$(CC) $(ICFLAGS) $(KERNELPATH) $(OCFLAGS) -o \
	$(BUILDPATH)kernel.o

$(BUILDPATH)interface.o: $(SOURCEPREF)interface.c
	$(CC) $(ICFLAGS) $(SOURCEPREF)interface.c $(OCFLAGS) -o \
	$(BUILDPATH)interface.o

$(BUILDPATH)vga.o: $(SOURCEPREF)vga.c
	$(CC) $(ICFLAGS) $(SOURCEPREF)vga.c $(OCFLAGS) -o \
	$(BUILDPATH)vga.o

$(BUILDPATH)gdt.o: $(SOURCEPREF)gdt.c
	$(CC) $(ICFLAGS) $(SOURCEPREF)gdt.c $(OCFLAGS) -o \
	$(BUILDPATH)gdt.o

test: all
	@if grub-file --is-x86-multiboot $(OUTPUTNAME); @then \
  	@echo multiboot working!!; \
	@else \
  	@echo can\'t find multiboot!!; \
	@fi

iso: all
	@grub-mkrescue -o $(BUILDPATH)nenos.iso ./iso

qemu: iso
	@qemu-system-i386 -cdrom $(BUILDPATH)nenos.iso

$(ASMPREFIX)boot.s: $(ASMPREFIX)isr_code_generator.rb
	ruby $(ASMPREFIX)isr_code_generator.rb

GDBPORT = $(shell expr `id -u` % 5000 + 25000)

QEMUGDB = $(shell if qemu-system-i386 -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

qemu-gdb: iso
	@qemu-system-i386 -cdrom $(BUILDPATH)nenos.iso -serial mon:stdio -S $(QEMUGDB)
