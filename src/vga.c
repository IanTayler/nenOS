#include <head.h>
#include <system.h>

#define BLACK   0
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED     4
#define MAGENTA 5
#define BROWN   6
#define L_GRAY  7
// For readability. Light WHITE is WHITE, so don't worry.
#define WHITE   15

// This is used to do all sorts of things.
#define CRT_ADDRESS 0x3d4
#define CRT_DATA    0x3d5

#define VGA_START   0xb8000
#define VGA_WIDTH   160
#define VGA_HEIGHT  24

typedef uchar Color;

/* Defining some global variables.
 * I consider this the correct way of handling this kind of
 * situation, so come at me global-haters
 */

int blinker_x = 0, blinker_y = 0;   // The current position of the
                                    // blinking cursor.
Color current_background = BLACK;
Color current_letter     = WHITE;


// There follows Color-handling functions.
Color light_version(Color color)
{
    // Set the high bit as 1, which makes the color brighter.
    return color | (1 << 3);
}

Color set_colors(Color background, Color letter)
{
    return letter | (background << 4);
}

Color current_colors()
{
    return set_colors(current_background, current_letter);
}


// Some automatized vga-buffer arithmetic
int position_from_coords(int x, int y)
{
    return (y * VGA_WIDTH/2 + x);
}

u16 *pointer_from_coords(int x, int y)
{
    return VGA_START + position_from_coords(x, y) * 2;
}

// Now the more general functions for handling the VGA buffer.
u16 get_printable(Color colors, uchar character)
{
    return character | (colors << 8);
}

u16 current_printable(uchar character)
{
    return get_printable(current_colors(), character);
}

void move_blinker()
{
    int blinker_place = position_from_coords(blinker_x, blinker_y);

    /* I won't remember how this works in a few months, so here comes
     * a pretty long explanation:
     * You write a certain number to CRT_ADDRESS, which is used to
     * designate a specific VGA operation you want to do. Then, You
     * pass an argument to that operation in CRT_DATA. Both are one byte
     * long.
     * In this case, we need to give the position of the blinker cursor
     * as argument, but we want to give it as two bytes, not one.
     * So, the correct way of doing that is first passing the low bits
     * as argument (CRT_DATA) and 0xe (i.e. 14) as the 'operation' to
     * perform (CRT_ADDRESS), and later we pass 0xf (i.e. 15) as the
     * 'operation' (CRT_ADDRESS) and the high 8 bits as argument to
     * CRT_DATA.
     */

     outportb(CRT_ADDRESS, 0xe);
     outportb(CRT_DATA, blinker_place >> 8);

     outportb(CRT_ADDRESS, 0xf);
     outportb(CRT_DATA, blinker_place);

     /* This is pretty unsafe, as we're writing a 32-bit value as
      * two 8-bit values. A rather weird method, but will have to do.
      */

}

/* Handle moving the blinker around */
void bl_inc_y()
{
    blinker_y++;
}

void bl_inc_x()
{
    if(blinker_x < (VGA_WIDTH - 1)) {
        blinker_x++;
    } else {
        blinker_x = 0;
        bl_inc_y();
    }
}

void bl_dec_y()
{
    if(blinker_y > 0)
        blinker_y--;
}

void bl_dec_x()
{
    if(blinker_x > 0) {
        blinker_x--;
    } else if (blinker_y > 0) {
        blinker_x = (VGA_WIDTH - 1);
        bl_dec_y();
    }
}

/* Basic operations with the full screen */
void scroll_if_needed()
{
    // Note this function doesn't call move_blinker()!!!
    if(blinker_y >= VGA_HEIGHT) {
        u16 blank = get_printable(current_background, ' ');

        // We start at the second line because we don't need to move
        // the first line.
        for(int line = 1; line <= VGA_HEIGHT; line++) {
            u16 *this_line  = pointer_from_coords(0, line);
            u16 *one_up     = pointer_from_coords(0, line-1);
            memcpy(one_up, this_line, VGA_WIDTH);
        }
        /* I commented the following lines of code  and things started
         * working. I have no idea why, though. */
        // Make the last line empty
        //memsetw(pointer_from_coords(0, VGA_HEIGHT), blank, VGA_WIDTH);
        blinker_y = VGA_HEIGHT - 1;
        //blinker_x = 0;
        move_blinker();
    }
}

void clear_screen()
{
    u16 blank = get_printable(current_background, ' ');
    for(int i = 0; i <= VGA_HEIGHT; i++)
        memsetw(pointer_from_coords(0, i), blank, VGA_WIDTH);
}

/* The stuff that actually prints */
void r_printchar(char c)
{
    if(c == 0x08) {
        bl_dec_x();;
    } else if(c == '\n' ) {
        blinker_x = 0;
        bl_inc_y();
    } else if(c >= ' ') {
        u16 content = current_printable(c);
        memsetw(pointer_from_coords(blinker_x, blinker_y),
                content, 1);
        bl_inc_x();
    }
    scroll_if_needed();
    move_blinker();
}

void r_print(char message[])
{
    for(int i = 0; message[i] != 0 ;i++)
        r_printchar(message[i]);
}

/* The useless */
void print_something()
{
    // Test that we're including our interface properly.
    // This prints 'OK' to the screen.
    uchar colors = set_colors(GREEN,light_version(RED));
    memsetw(VGA_START, get_printable(colors, 'O'), 1);
    memsetw(VGA_START + 2, get_printable(colors, 'K'), 1);
}

void blinker_something()
{
    blinker_x = 2;
    blinker_y = 0;
    move_blinker();
}
