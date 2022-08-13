#include "console.h"
#include "multiboot.h"
#include "idt.h"
#include "timer.h"
#include "elf.h"
int entry(struct multiboot_t *m)
{
    console_printf("HelloOS!");
    load_idt();
    init_8259();
    init_timer(100);
    init_sym(m);
    asm volatile("sti");
    while (1)
        ;
    return 0;
}