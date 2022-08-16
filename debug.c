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
void panic_spin(char *filename,
                int line,
                const char *func,
                const char *condition)
{
    asm volatile("cli");
    console_printf("\n\n\n!!!!! error !!!!!\n");
    console_printf("filename:");
    console_printf(filename);
    console_printf("\n");
    console_printf("line:0x");
    console_printf(line);
    console_printf("\n");
    console_printf("function:");
    console_printf((char *)func);
    console_printf("\n");
    console_printf("condition:");
    console_printf((char *)condition);
    console_printf("\n");
    while (1)
        ;
}
