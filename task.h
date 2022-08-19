#ifndef TASK_H
#define TASK_H
#define __MAX_TASK_NUM 10
#include "lib/types.h"
struct Task
{
    int tid;
    int eax;
    int ebx;
    int ecx;
    int edx;
    int edi;
    int esi;
    int esp;
    int ebp;
    int eip;
    int eflags;
    int cr3;
    int valid;
    int time_ticket;
};
struct TSS
{
    uint32_t backlink;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap;
};
struct TSS_DESCRIPTOR
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t type : 4;
    uint8_t : 1;
    uint8_t dpl : 2;
    uint8_t p : 1;
    uint8_t limit_high : 4;
    uint8_t avl : 1;
    uint8_t : 2;
    uint8_t granularity : 1;
    uint8_t base_high;
};
struct Task task_list[__MAX_TASK_NUM];
struct TSS r0_tss;
struct TSS_DESCRIPTOR r0_tss_desc;
void save_r0_tss();
void schdule(int esp, int ebp, int edi, int esi, int edx, int ecx, int ebx, int eax, int eip, int cs, int eflags);
void init_task();
uint32_t start_task(int func, int stack);
void sleep(int s);
void exit_task();
uint32_t find_tid();
extern void switch_task(int esp, int eip, int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp, int eflags);
#endif