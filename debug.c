#include "debug.h"
#include "types.h"
#include "elf.h"
#include "console.h"
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
void panic(char *str)
{
    console_write_color("\n\nPANIC: ", rc_black, rc_red);
    console_write_color(str, rc_black, rc_red);
    console_write_color("\n\n", rc_black, rc_red);
    print_stack_trace();
    asm volatile("cli");
    while (1)
        ;
}