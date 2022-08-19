#include "proc.h"
void sleep(uint32_t s)
{
    asm volatile("mov %0, %%ebx"
                 :
                 : "r"(s));
    asm volatile("mov $1, %eax");
    asm volatile("int $49");
    asm volatile("int $32");
}
void exitthread()
{
    asm volatile("mov $2, %eax");
    asm volatile("int $49");
    asm volatile("int $32");
}
// 启动线程，成功返回tid，失败返回0
uint32_t createthread(void *entry, uint32_t esp)
{
    int eax;
    asm volatile("mov %0, %%ebx"
                 :
                 : "r"(entry));
    asm volatile("mov %0, %%ecx"
                 :
                 : "r"(esp));
    asm volatile("mov $6, %eax");
    asm volatile("int $49");
    // return eax
    asm volatile("mov %%eax, %0"
                 : "=r"(eax));
    return eax;
}