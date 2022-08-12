#include "console.h"
#include "multiboot.h"
int entry(struct multiboot_t *m)
{
    console_printf("HelloOS!");
    while (1)
        ;
    return 0;
}