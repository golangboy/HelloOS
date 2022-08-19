#include "./include/debug.h"
#include "../lib/include/types.h"
#include "../lib/include/elf.h"
#include "./include/console.h"
elf_debug_t elf;
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
    console_printf("line:%x", line);
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
void init_sym(struct multiboot_t *m)
{
    elf_section_header_t *sec = (elf_section_header_t *)m->addr;
    int nametable = sec[m->shndx].addr;
    for (int i = 0; i < m->num; i++)
    {
        char *name = (char *)(sec[i].name + (uint32_t)nametable);
        // console_printf("%d - %s \n", i, name);
        if (strcmp(name, ".symtab") == 0)
        {

            elf.symtab = (elf_symbol_t *)sec[i].addr;
            elf.symtabsz = sec[i].size;
        }
        else if (strcmp(name, ".strtab") == 0)
        {
            elf.strtab = (const char *)sec[i].addr;
            elf.strtabsz = sec[i].size;
        }
    }
}

char *lookup_sym(uint32_t addr)
{
    for (int i = 0; i < elf.symtabsz / (sizeof(struct elf_symbol_t)); i++)
    {
        if (addr >= elf.symtab[i].value && addr < elf.symtab[i].value + elf.symtab[i].size)
        {
            return (char *)(elf.strtab + elf.symtab[i].name);
        }
    }
    return 0;
}