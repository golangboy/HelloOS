#ifndef USER_H
#define USER_H
#define __MAX_PCB_NUM 20
#define __MAX_TASK_NUM 20
#include "types.h"
struct PCB
{
    char uname[10];
    int valid;
    uint32_t cr3;
    uint32_t pid;
    uint32_t tasks[__MAX_TASK_NUM];
};
struct PCB pcb_list[__MAX_PCB_NUM];

uint32_t create_pcb(char *uname, void *entry);
uint32_t getpidbytid(uint32_t tid);
uint32_t findfreepid();
struct PCB *getpcbbypid(uint32_t pid);
#endif