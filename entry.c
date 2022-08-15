#include "console.h"
#include "multiboot.h"
#include "idt.h"
#include "timer.h"
#include "elf.h"
void taskA()
{
    while (1)
    {
        console_printf("A");
        sleep(1);
    }
}
void taskB()
{
    for (int i = 0; i < 2; i++)
    {
        console_printf("B");
        sleep(5);
    }
}
void taskC()
{
    for (int i = 0; i < 200; i++)
    {
        console_printf("C");
        sleep(1);
    }
}
int entry(struct multiboot_t *m)
{
    console_printf("HelloOS!");
    load_idt();
    init_8259();
    init_timer();
    init_sym(m);
    start_task(taskA, 0x200000);
    start_task(taskB, 0x200000 + 0x1000);
    start_task(taskC, 0x200000 + 0x2000);
    while (1)
    {
        asm volatile("hlt");
    }
    return 0;
}