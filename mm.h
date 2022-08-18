#ifndef MM_H
#define MM_H
#include "multiboot.h"
#define __MAX_MEMBK_CNT 100
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
struct MEM_BLOCK
{
    uint32_t pid;
    uint64_t start_addr;
    uint64_t size;
};
struct MEM_MG
{
    struct MEM_BLOCK freemem[__MAX_MEMBK_CNT], allocmem[__MAX_MEMBK_CNT];
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
void page_falut();
struct PDE *kernel_pde_entry;
#endif