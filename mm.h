#ifndef MM_H
#define MM_H
#include "multiboot.h"
#define MAX_MEMBK_CNT 100
struct PDE
{
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t unused : 7;
    uint32_t frame : 20;
};
struct PTE
{
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t unused : 7;
    uint32_t frame : 20;
};
struct Mem_Free_Info
{
    uint64_t start_addr;
    uint64_t size;
};
struct Mem_Mg
{
    struct Mem_Free_Info freemem[MAX_MEMBK_CNT], allocmem[MAX_MEMBK_CNT];
};
void print_memory_map(struct multiboot_t *m);
void add_memmg(uint64_t addr, uint64_t size);
void *alloc(uint64_t size);
void *alloc_4k(uint64_t size);
int free(void *ptr);
void merge();
uint64_t get_freemem();
uint64_t get_allocmem();
void mg_info();
#endif