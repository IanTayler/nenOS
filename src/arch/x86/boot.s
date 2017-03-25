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
    jmp stublet

mboot:
    .align 4
    # Here we set up multiboot information.
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

    movl _sys_stack, %esp

stublet:
    # Print some stuff.
    #movl $0x2f4b2f4f, 0xb8000
    call cmain
    cli
lp:
    hlt
    jmp lp

.global gdt_flush
.extern gp
gdt_flush:
    lgdt gp
    movw $0x10, %ax
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    ljmp $0x08, $flush2
flush2:
    ret

.section .bss
.align 0x10
_sys_stack_bott:
    .skip 0x4000
_sys_stack:
