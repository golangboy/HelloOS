#include "proc.h"
void sleep(int s)
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