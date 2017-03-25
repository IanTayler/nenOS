#include <head.h>
#include <system.h>

uchar *memcpy(uchar *dest, const uchar *src, int count)
{
    /* Add code here to copy 'count' bytes of data from 'src' to
    *  'dest', finally return 'dest' */
    uchar *new_dest = dest;
    const uchar *new_src = src;

    for(; count > 0; count--) {
        *new_dest++ = *new_src++;
    }
    return dest;
}

uchar *memset(uchar *dest, uchar val, int count)
{
    /* Add code here to set 'count' bytes in 'dest' to 'val'.
    *  Again, return 'dest' */
    uchar *new_dest = dest;

    for(; count > 0; count--) {
        *new_dest++ = val;
    }
    return dest;
}

u16 *memsetw(u16 *dest, u16 val, int count)
{
    /* Same as above, but this time, we're working with a 16-bit
    *  'val' and dest pointer. Your code can be an exact copy of
    *  the above, provided that your local variables if any, are
    *  u16 */
    u16 *new_dest = dest;

    for(; count > 0; count--) {
        *new_dest++ = val;
    }
    return dest;
}

int strlen(const char *str)
{
    /* This loops through character array 'str', returning how
    *  many characters it needs to check before it finds a 0.
    *  In simple words, it returns the length in bytes of a string */
    int ret_val = 0;
    for(; str[ret_val] != '\00'; ret_val++);
    return ret_val;
}

/* We will use this later on for reading from the I/O ports to get data
*  from devices such as the keyboard. We are using what is called
*  'inline assembly' in these routines to actually do the work */
uchar inportb (u16 _port)
{
    uchar rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

/* We will use this to write to I/O ports to send bytes to devices. This
*  will be used in the next tutorial for changing the textmode cursor
*  position. Again, we use some inline assembly for the stuff that simply
*  cannot be done in C */
void outportb (u16 _port, uchar _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}
