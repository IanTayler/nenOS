TOOLPREFIX		= i686-elf-
ARCH			= x86

CC				= $(TOOLPREFIX)gcc
LD				= $(CC)
AS				= $(TOOLPREFIX)as

OUTPUTPREF		= ./iso/boot/
OUTPUTNAME 		= $(OUTPUTPREF)nenos.bin

BUILDPATH		= ./build/

SOURCEPREF		= ./src/
INCLUDEPREF		= $(SOURCEPREF)include/

# Inner C flags.
ICFLAGS 		= -I$(SOURCEPREF) -I$(INCLUDEPREF) -c
# Outer C flags.
OCFLAGS 		= -ffreestanding -O2 -Wall -Wextra

ILDFLAGS 		= -T $(LINKSCRIPT) -ffreestanding -O2 -nostdlib
OLDFLAGS 		= -o $(OUTPUTNAME) -lgcc

ASMPREFIX		= ./src/arch/$(ARCH)/

INCLUSIONS		= $(INCLUDEPREF)system.h $(INCLUDEPREF)head.h
SOURCES			= $(SOURCEPREF)interface.c

KERNELPATH		= $(SOURCEPREF)kernel.c

LINKSCRIPT		= $(ASMPREFIX)link.ld

.PHONY: all test iso

all: $(BUILDPATH)boot.o $(BUILDPATH)kernel.o
	$(LD) $(ILDFLAGS) $(BUILDPATH)boot.o $(BUILDPATH)kernel.o $(OLDFLAGS)

$(BUILDPATH)boot.o: $(ASMPREFIX)boot.s
	$(AS) -c $(ASMPREFIX)boot.s -o $(BUILDPATH)boot.o

$(BUILDPATH)kernel.o: $(KERNELPATH) $(INCLUSIONS)\
					  $(BUILDPATH)interface.o
	$(CC) $(ICFLAGS) $(KERNELPATH) $(OCFLAGS) -o \
	$(BUILDPATH)kernel.o

$(BUILDPATH)interface.o:
	$(CC) $(ICFLAGS) $(KERNELPATH) $(OCFLAGS) -o \
	$(BUILDPATH)interface.o

test: all
	if grub-file --is-x86-multiboot $(OUTPUTNAME); then \
  	echo multiboot working!!; \
	else \
  	echo can\'t find multiboot!!; \
	fi

iso: all
	grub-mkrescue -o $(BUILDPATH)nenos.iso ./iso

qemu: iso
	qemu-system-i386 -cdrom $(BUILDPATH)nenos.iso
