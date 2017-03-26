#include <head.h>

#ifndef __SYSTEM_H
#define __SYSTEM_H

/* INTERFACE.C */
extern uchar *memcpy(uchar *dest, const uchar *src, int count);
extern uchar *memset(uchar *dest, uchar val, int count);
extern u16 *memsetw(u16 *dest, u16 val, int count);
extern int strlen(const char *str);
extern uchar inportb (u16 _port);
extern void outportb (u16 _port, uchar _data);

/* VGA.C */
extern void print_something();
extern void blinker_something();
extern void r_print(char message[]);
extern void clear_screen();

/* GDT.C */
extern void gdt_install();
extern struct gdt_entry new_gdt_entry(u32 base, u32 limit, uchar privilege, uchar dir_or_control, uchar read_write, uchar exec, uchar granularity);
extern void gdt_set_gate(int num, struct gdt_entry entry);
extern void gdt_test();

extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

#endif
