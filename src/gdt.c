#include <head.h>
#include <arch.h>
#include <system.h>
/* I rewrote the whole file by hand. This isn't exactly 64-bit
 * compatible. Perhaps we could make it so with enough
 * Makefile-twiddling. The name can be misleading, as this actually
 * also includes the IDT */

/***********************
 * CHANGABLE CONSTANTS *
 ***********************/
 #define ENTRIES    5

/***************
 * DEFINITIONS *
 ***************/
#define KERNELPRIV  0b0
#define USERPRIV    0b11

#define CODEMODE    0b1
#define DATAMODE    0b0

#define GROWSUP     0b0
#define GROWSDOWN   0b1

#define LAXCTRL     0b1
#define TIGHTCTRL   0b0

#define CANRW       0b1
#define CANNOTRW    0b0

#define BYTEGRAN    0b0
#define PAGEGRAN    0b1

struct gdt_entry
{
    u32 base;
    u32 limit;
    uchar privilege;
    uchar dir_or_control;
    uchar read_write;
    uchar exec;
    uchar granularity;

};

struct packed_entry
{
    u32 firstpart;
    u32 secondpart;
};
/* Special pointer which includes the limit: The max bytes
*  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct gdt_ptr
{
    u16 limit;
    u32 base;
} __attribute__((packed));

/* Our GDT, with ENTRIES entries, and finally our special GDT pointer*/
struct packed_entry gdt[3];
struct gdt_ptr gp;

/* This will be a function in start.asm. We use this to properly
*  reload the new segment registers */
extern void gdt_flush();

/* Create a C-internal representation for a GDT entry */
struct gdt_entry new_gdt_entry(u32 base, u32 limit, uchar privilege, uchar dir_or_control, uchar read_write, uchar exec, uchar granularity)
{
    struct gdt_entry ret_val = {base, limit, privilege, dir_or_control, read_write, exec, granularity};
    return ret_val;
}

/* Now we have to create the 64-bit representation of the entry.
 * Let's explain how this works:
 * The main point to understand is that by bitwise-&-ing the values
 * of the variables, we force them to a certain size. Rember two
 * hexadecimal digits are equivalent to one byte */
struct packed_entry repr_from_gdt_entry(struct gdt_entry entry)
{
    // This may or may not be terribly bugged.
    u16 first_word      = entry.limit & 0xffff;
    u16 second_word     = entry.base & 0xffff;

    // We need bits 16-23 of base for this one. Pay attention.
    uchar third_char    = (entry.base >> 16) & 0xff;

    // Now things have gotten truly horrible.
    //###### WE MOVED IT ONCE!!!!
    uchar fourth_char   = 0xff &
                          (0x90
                            | ((entry.read_write)     << 1)
                            | ((entry.dir_or_control) << 2)
                            | ((entry.exec)           << 3)
                            | ((entry.privilege)      << 5)
                        );

    uchar fifth_char    = 0xff &
                          ((GDT_ROOT << 4)
                            | (entry.granularity << 7)
                            | ((entry.limit >> 16))
                        );

    // Back to normal.
    uchar sixth_char    = (entry.base >> 24) & 0xff;

    struct packed_entry ret_val = {
            (first_word & 0xffffffff)           |
            ((second_word & 0xffffffff) << 16),

            ((third_char  & 0xffffffff))        |
            ((fourth_char & 0xffffffff) << 8)   |
            ((fifth_char  & 0xffffffff) << 16)  |
            ((sixth_char  & 0xffffffff) << 24)};
    return ret_val;

}

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, struct gdt_entry entry)
{
    /* Setup the descriptor base address */
    gdt[num] = repr_from_gdt_entry(entry);
}

/* Should be called by main. This will setup the special GDT
*  pointer, set up the first 3 entries in our GDT, and then
*  finally call gdt_flush() in our assembler file in order
*  to tell the processor where the new GDT is and update the
*  new segment registers */

void gdt_install()
{
    /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct packed_entry) * 3) - 1;
    gp.base = gdt;

    /* Our NULL descriptor */
    gdt_set_gate(0, new_gdt_entry(0, 0, 0, 0, 0, 0, 0));

    /* Code segment */
    gdt_set_gate(1, new_gdt_entry(
                    0x0000000,     // at 16MiB normally
                    0x1000,        // Until 8GiB
                    KERNELPRIV,
                    TIGHTCTRL,
                    CANRW,
                    CODEMODE,
                    PAGEGRAN
                    )
                );

    /* Data segment */
    gdt_set_gate(2, new_gdt_entry(
                    0x00000000,     // at 0MiB
                    0x1000,         // Until 16MiB
                    KERNELPRIV,
                    TIGHTCTRL,
                    CANRW,
                    DATAMODE,
                    PAGEGRAN
                    )
                );

    /* Flush out the old GDT and install the new changes! */
    gdt_flush();
}

char* itoa(u32 i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;

    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

/*###################
 * THIS IS THE TEST *
 *###################*/
void gdt_test()
{
    char buff[100];
    char bufft[100];
    r_print(itoa(repr_from_gdt_entry(new_gdt_entry(
                    0x0100000,      // at 0MiB
                    0x1000,         // Until 4Gib
                    KERNELPRIV,
                    TIGHTCTRL,
                    CANRW,
                    DATAMODE,
                    PAGEGRAN
                )).firstpart, buff));
    r_print("\n");
    r_print(itoa(sizeof(struct packed_entry), buff));
    r_print("\n");
    r_print(itoa(repr_from_gdt_entry(new_gdt_entry(
                    0x01000000,     // at 0MiB
                    0x1000,         // Until 4Gib
                    KERNELPRIV,
                    TIGHTCTRL,
                    CANRW,
                    DATAMODE,
                    PAGEGRAN
                )).secondpart, bufft));
    r_print("\n");
}

/****************************
 * HERE STARTS THE IDT CODE *
 ****************************/

/* Defines an IDT entry */
struct idt_entry
{
    u16 base_lo;
    u16 sel;        /* Our kernel segment goes here! */
    uchar always0;     /* This will ALWAYS be set to 0! */
    uchar flags;       /* Set using the above table! */
    u16 base_hi;
} __attribute__((packed));

struct idt_ptr
{
    u16 limit;
    u32 base;
} __attribute__((packed));

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries in this tutorial, the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
struct idt_entry idt[256];
struct idt_ptr idtp;

/* This exists in 'start.asm', and is used to load our IDT */
extern void idt_load();

/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void idt_set_gate(uchar num, u32 base, u16 sel, uchar flags)
{
    /* We'll leave you to try and code this function: take the
    *  argument 'base' and split it up into a high and low 16-bits,
    *  storing them in idt[num].base_hi and base_lo. The rest of the
    *  fields that you must set in idt[num] are fairly self-
    *  explanatory when it comes to setup */
    idt[num].base_lo    = base & 0xffff;
    idt[num].base_hi    = (base >> 16) & 0xffff;

    idt[num].sel        = sel;
    idt[num].flags      = flags;

    idt[num].always0    = 0xff;
}

/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = &idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    /* Add any new ISRs to the IDT here using idt_set_gate */

    /* Points the processor's internal register to the new IDT */
    idt_load();
}
