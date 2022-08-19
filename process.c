#include "process.h"
#include "lib/strings.h"
#include "mm.h"
#include "debug.h"
#include "task.h"
#include "console.h"
void init_pcb_list()
{
    for (int i = 0; i < __MAX_PCB_NUM; i++)
    {
        pcb_list[i].valid = 0;
    }
}
// 根据任务id返回pid，如果找不到返回0，代表是内核任务
uint32_t getpidbytid(uint32_t tid)
{
    for (int i = 0; i < __MAX_PCB_NUM; i++)
    {
        if (pcb_list[i].valid == 1) //进程有效
        {
            for (int j = 0; j < __MAX_PCBTASK_NUM; j++)
            {
                if (pcb_list[i].tasks[j].tid == tid && pcb_list[i].tasks[j].valid == 1)
                {
                    return pcb_list[i].pid;
                }
            }
        }
    }
    return 0;
}
// uname 进程名 entry主线程的入口地址
uint32_t create_pcb(char *uname, void *entry)
{
    uint32_t new_pid = find_freepid();
    ASSERT(new_pid != -1);
    struct PDE *pde_entry = (struct PDE *)alloc_4k(1024 * 4);
    ASSERT(pde_entry != 0);
    for (int i = 0; i < 1024; i++)
    {

        pde_entry[i].accessed = 0;
        pde_entry[i].dirty = 0;
        pde_entry[i].user = 0;
        pde_entry[i].present = 0;
        pde_entry[i].frame = 0;
        pde_entry[i].rw = 0;
        pde_entry[i].user = 0;
    }
    pde_entry[0] = kernel_pde_entry[0];
    for (int i = 0x300; i < 1024; i++)
    {
        pde_entry[i] = kernel_pde_entry[i];
    }
    for (int i = 0; i < __MAX_PCB_NUM; i++)
    {
        if (pcb_list[i].valid == 0)
        {
            strcpy(pcb_list[i].uname, uname);
            pcb_list[i].valid = 1;
            pcb_list[i].pid = new_pid;
            pcb_list[i].cr3 = ((int)pde_entry);

            // init task sets
            for (int j = 0; j < __MAX_TASK_NUM; j++)
            {
                pcb_list[i].tasks[j].valid = 0;
            }
            pcb_list[i].tasks[0].valid = 1;
            pcb_list[i].tasks[0].tid = start_task(entry, 0x4000000);
            return new_pid;
        }
    }
    panic("no space for new pcb");
}
// 获取一个空闲的pid
uint32_t find_freepid()
{
    uint32_t pid = 0;
    for (int i = 0; i < __MAX_PCB_NUM; i++)
    {
        if (pcb_list[i].valid == 1)
        {
            if (pcb_list[i].pid > pid)
            {
                pid = pcb_list[i].pid;
            }
        }
    }
    return pid + 1;
}
// 根据pid获取pcb
struct PCB *getpcbbypid(uint32_t pid)
{
    for (int i = 0; i < __MAX_PCB_NUM; i++)
    {
        if (pcb_list[i].valid == 1 && pcb_list[i].pid == pid)
        {
            return &pcb_list[i];
        }
    }
    return 0;
}
// 获取当前进程pid，如果是内核返回0
uint32_t getcurpid()
{
    uint32_t cr3 = 0;
    asm volatile("mov %%cr3, %0"
                 : "=r"(cr3));
    for (int i = 0; i < __MAX_PCB_NUM; i++)
    {
        if (pcb_list[i].valid == 1 && pcb_list[i].cr3 == cr3)
        {
            return pcb_list[i].pid;
        }
    }
    return 0;
}