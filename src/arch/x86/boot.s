# Declare constants for the multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# We set up our multiboot flags at the start of the .text section
# instead of on a different section. I hope this is fine!
.section .text
.global _start
.type _start @function
_start:

mboot:
    .align 4
    # Here we set up multiboot information.
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

    # A.OUT 'kludge' (i.e. weird hack)
    # Will comment and see if ELF just works here.
    #dd mboot
    #dd code
    #dd bss
    #dd end
    #dd _start

    movl _sys_stack, %esp
    jmp stublet

stublet:
    call cmain
    cli
lp:
    hlt
    jmp lp

.section .bss
.align 0x10
_sys_stack_bott:
    .skip 0x4000
_sys_stack:
