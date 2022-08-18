#include "task.h"
#include "timer.h"
#include "console.h"
#include "types.h"
#include "process.h"
#include "mm.h"
struct Task task_list[__MAX_TASK_NUM];
int curtask_idx = -1;
int first_task = 1;
void task_finish(int task_idx);
uint32_t start_task(int func, int stack)
{
    asm volatile("cli");
    for (int i = 1; i < __MAX_TASK_NUM; i++)
    {
        if (task_list[i].valid == 0)
        {
            task_list[i].valid = 1;
            task_list[i].esp = stack;
            task_list[i].eip = func;
            task_list[i].eax = 0;
            task_list[i].ebx = 0;
            task_list[i].ecx = 0;
            task_list[i].edx = 0;
            task_list[i].esi = 0;
            task_list[i].edi = 0;
            task_list[i].ebp = 0;
            task_list[i].tid = find_tid();
            return task_list[i].tid;
        }
    }
    asm volatile("sti");
}
void init_task()
{
    for (int i = 0; i < __MAX_TASK_NUM; i++)
    {
        task_list[i].tid = 0;
        task_list[i].valid = 0;
        task_list[i].time_ticket = 0;
    }
}
void exit_task()
{
    asm volatile("cli");
    task_list[curtask_idx].valid = 0;
    asm volatile("sti");
    while (1)
        ;
}

void schdule(int esp, int ebp, int edi, int esi, int edx, int ecx, int ebx, int eax, int eip, int cs, int eflags)
{
    uint32_t cur_tid = curtask_idx;
    uint32_t cur_pid = getpidbytid(cur_tid);
    if (first_task == 1)
    {
        curtask_idx = 0;
        first_task = 0;
        task_list[curtask_idx].valid = 1;
    }
    task_list[curtask_idx].eax = eax;
    task_list[curtask_idx].ebx = ebx;
    task_list[curtask_idx].ecx = ecx;
    task_list[curtask_idx].edx = edx;
    task_list[curtask_idx].esi = esi;
    task_list[curtask_idx].edi = edi;
    task_list[curtask_idx].ebp = ebp;
    task_list[curtask_idx].esp = esp;
    task_list[curtask_idx].eip = eip;
    task_list[curtask_idx].eflags = eflags;
    int cur_time = get_curtime();
    for (int i = 0; i < __MAX_TASK_NUM; i++)
    {
        if (task_list[i].valid == 1 && task_list[i].time_ticket <= cur_time)
        {
            task_list[i].time_ticket = 0;
        }
    }
    //寻找下一个可运行的任务
    int old_idx = curtask_idx;
    for (int i = 0, k = 1; i < __MAX_TASK_NUM; i++, k++)
    {
        int next_task_idx = (k + curtask_idx) % __MAX_TASK_NUM;
        if (task_list[next_task_idx].valid == 1)
        {
            if (task_list[next_task_idx].time_ticket == 0)
            {
                curtask_idx = next_task_idx;
                break;
            }
        }
    }
    uint32_t next_tid = curtask_idx;
    uint32_t next_pid = getpidbytid(next_tid);
    // if (0 != cur_tid && 0 != next_tid)
    // console_printf("task %d(%d) switch to task %d(%d)\n", cur_tid, cur_pid, next_tid, next_pid);
    if (0 == next_pid)
    {
        uint32_t cr3 = (uint32_t)kernel_pde_entry;
        asm volatile("mov %0, %%cr3" ::"r"(cr3));
    }
    else
    {
        struct PCB *pcb = getpcbbypid(next_pid);
        uint32_t cr3 = (uint32_t)pcb->cr3;
        asm volatile("mov %0, %%cr3" ::"r"(cr3));
    }
    switch_task(task_list[curtask_idx].esp, task_list[curtask_idx].eip, task_list[curtask_idx].eax, task_list[curtask_idx].ebx, task_list[curtask_idx].ecx, task_list[curtask_idx].edx, task_list[curtask_idx].esi, task_list[curtask_idx].edi, task_list[curtask_idx].ebp, task_list[curtask_idx].eflags);
}

void sleep(int s)
{
    asm volatile("cli");

    task_list[curtask_idx].time_ticket = get_curtime() + s;

    asm volatile("sti");
    //立刻中断调度
    asm volatile("int $32");
}

uint32_t find_tid()
{
    uint32_t tid = 0;
    for (int i = 0; i < __MAX_TASK_NUM; i++)
    {
        if (task_list[i].valid == 1)
        {
            if (tid < task_list[i].tid)
            {
                tid = task_list[i].tid;
            }
        }
    }
    return tid + 1;
}