#include <head.h>
#include <system.h>

void cmain()
{
    gdt_test();
    gdt_install();
    // Testing our interfaces.
    //r_print("Hello, dot. What are you doing? I'm here with my family dancing around a little fire! Yeah!\nWhy don't we dance around?\n");
    r_print("Be the one!\n");
    r_print("Dance around!\n");
    r_print("Yeah!\n");
    r_print("Hell yeah!");
    for(;;);
}
