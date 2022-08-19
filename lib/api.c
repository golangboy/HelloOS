#include "api.h"
void test()
{
    asm volatile("movl $0, %eax");
    asm volatile("int $49");
}
void sleep(int s)
{
    asm volatile("mov %0, %%ebx"
                 :
                 : "r"(s));
    asm volatile("mov $1, %eax");
    asm volatile("int $49");
    asm volatile("int $32");
}