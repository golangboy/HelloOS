#include "debug.h"
#include "types.h"
#include "elf.h"
void print_stack_trace()
{
    uint32_t *ebp, *eip;

    asm volatile("mov %%ebp, %0"
                 : "=r"(ebp));
    while (ebp)
    {
        eip = ebp + 1;
        console_printf("   [%x] %s\n", *eip, lookup_sym(*eip));
        ebp = (uint32_t *)*ebp;
    }
}