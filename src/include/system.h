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

#endif
