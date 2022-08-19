#include "lib/types.h"
#include "lib/syscall.h"
#include "console.h"
#include "task.h"
#include "process.h"
int syscall(int eax, int ebx, int ecx, int edx, int edi, int esi)
{
    uint32_t tid = get_curtid();
    uint32_t pid = getpidbytid(tid);
    if (eax == __SYSCALL_TEST)
    {
        console_printf("[System Call Test:%d]\n", ebx);
        return 0x11223344;
    }
    else if (eax == __SYSCALL_SLEEP)
    {
        struct Task *cur_task = gettask_bytid(tid);
        cur_task->time_ticket = get_curtime() + ebx;
        return 0;
    }
    return 1;
}