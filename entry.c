#include "./kernel/include/console.h"
#include "./kernel/include/multiboot.h"
#include "./kernel/include/idt.h"
#include "./kernel/include/timer.h"
#include "./lib/include/elf.h"
#include "./kernel/include/mm.h"
#include "./kernel/include/debug.h"
#include "./kernel/include/task.h"
#include "./kernel/include/process.h"
#include "./kernel/include/pic.h"
#include "./user/include/init.h"
void taskA()
{
    while (1)
    {
        console_printf("Kernel_Thread:A\n");
        sleep_kernel(1);
    }
    exit_task();
}
void taskB()
{
    for (int i = 0; i < 2; i++)
    {
        console_printf("B");
        sleep_kernel(5);
    }
    panic("Alloc fail");
    exit_task();
}
void taskC()
{
    for (int i = 0; i < 200; i++)
    {
        console_printf("C");
        sleep_kernel(1);
    }
    exit_task();
}
int entry(struct multiboot_t *m)
{
    console_printf("HelloOS!\n");
    print_memory_map(m);
    init_vm();
    load_idt();
    init_8259();
    init_timer();
    init_sym(m);
    mg_info();
    asm volatile("cli");
    // start_task((int)taskA, (int)((int)alloc(1024) + 1023));
    // start_task(taskB, (int)((int)alloc(1024) + 1023));
    // start_task(taskC, (int)((int)alloc(1024) + 1023));
    console_printf("Initial Finish!\n");
    int pid = create_pcb("father", init);
    console_printf("Create init pid: %d\n", pid);
    asm volatile("sti");
    while (1)
    {
        asm volatile("hlt");
    }
    return 0;
}