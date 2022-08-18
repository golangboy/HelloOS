#ifndef TASK_H
#define TASK_H
#define MAX_TASK_NUM 10
struct Task
{
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
void schdule(int esp, int ebp, int edi, int esi, int edx, int ecx, int ebx, int eax, int eip, int cs, int eflags);
void init_task();
void start_task(int func, int stack);
void sleep(int s);
void exit_task();
extern void switch_task(int esp, int eip,int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp, int eflags);
#endif