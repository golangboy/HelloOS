#include "lib/types.h"
#include "syscall.h"
#include "console.h"
#include "task.h"
#include "process.h"
#include "timer.h"
int syscall(int eax, int ebx, int ecx, int edx, int edi, int esi)
{
    uint32_t tid = get_curtid();
    uint32_t pid = get_pid_bytid(tid);
    if (eax == __SYSCALL_TEST)
    {
        console_printf("[System Call Test:%d]\n", ebx);
        return 0x11223344;
    }
    else if (eax == __SYSCALL_SLEEP)
    {
        struct Task *cur_task = get_task_bytid(tid);
        cur_task->time_ticket = get_curtime() + ebx;
        return 0;
    }
    else if (eax == __SYSCALL_EXITTASK)
    {
        struct Task *cur_task = get_task_bytid(tid);
        //不再调度这个线程
        cur_task->valid = 0;
        struct PCB *cur_pcb = get_pcb_bypid(pid);
        //所有线程是否已经退出
        int all_exit = 1;
        for (int i = 0; i < __MAX_PCBTASK_NUM; i++)
        {
            if (cur_pcb->tasks[i].tid == tid)
            {
                cur_pcb->tasks[i].valid = 0;
            }
            if (cur_pcb->tasks[i].valid == 1)
            {
                all_exit = 0;
                break;
            }
        }
        if (all_exit)
        {
            console_printf("Free PID:%d\n", pid);
            free_process(pid);
        }
        return 0;
    }
    else if (eax == __SYSCALL_PUTS)
    {
        console_printf("%s", ebx);
        return 0;
    }
    else if (eax == __SYSCALL_CLEAR)
    {
        console_clear();
        return 0;
    }
    return 1;
}