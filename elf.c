#include "elf.h"
elf_t elf;
void init_sym(struct multiboot_t *m)
{
    elf_section_header_t *sec = (elf_section_header_t *)m->addr;
    int nametable = sec[m->shndx].addr;
    for (int i = 0; i < m->num; i++)
    {
        char *name = (const char *)(sec[i].name + (uint32_t)nametable);
        //console_printf("%d - %s \n", i, name);
        if (strcmp(name, ".symtab") == 0)
        {

            elf.symtab = sec[i].addr;
            elf.symtabsz = sec[i].size;
        }
        else if (strcmp(name, ".strtab") == 0)
        {
            elf.strtab = sec[i].addr;
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
            return elf.strtab + elf.symtab[i].name;
        }
    }
    return 0;
}