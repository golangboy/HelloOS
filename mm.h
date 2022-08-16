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
    int start_addr;
    int size;
};
struct Mem_Mg
{
    struct Mem_Free_Info freemem[MAX_MEMBK_CNT], allocmem[MAX_MEMBK_CNT];
};
void print_memory_map(struct multiboot_t *m);
void add_memmg(int addr, int size);
void *alloc(int size);
void *alloc_4k(int size);
int free(void *ptr);
void merge();
int get_freemem();
int get_allocmem();
void mg_info();
#endif