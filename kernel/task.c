#include "./include/task.h"
#include "./include/timer.h"
#include "./include/console.h"
#include "../lib/include/types.h"
#include "./include/process.h"
#include "./include/mm.h"
#include "../lib/include/strings.h"
#include "./include/debug.h"
int curtask_idx = -1;
int first_task = 1;
//创建线程，成功返回tid，失败返回0
uint32_t start_task(int func, int stack)
{
    uint32_t pid = get_curpid();
    struct PCB *pcb = get_pcb_bypid(pid);
    int hasspace = 0, idx = 0;
    ASSERT(pid == 0 || (pcb != 0))
    //是否已经达到最大的线程数
    for (int i = 0; i < __MAX_PCBTASK_NUM; i++)
    {
        if (pcb->tasks[i].valid == 0)
        {
            hasspace = 1;
            idx = i;
            break;
        }
    }
    if (hasspace == 0 && pid != 0)
    {
        //用户进程的线程数已经达到最大
        // console_printf("No space for new task!\n");
        return 0;
    }
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
            pcb->tasks[idx].tid = task_list[i].tid;
            pcb->tasks[idx].valid = 1;
            return task_list[i].tid;
        }
    }
    return 0;
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
void before_schdule(int eflags, int cs, int edi, int esi, int edx, int ecx, int ebx, int eax, int ebp, int esp, int eip)
{
    if (eflags & (0x200))
    {
        //关中断
        eflags = eflags ^ 0x200;
    }
    console_printf("cflags: %d cs : %d\n", eflags, cs);
    schdule(esp + 4, ebp, edi, esi, edx, ecx, ebx, eax, eip, cs, eflags);
    // never reach here
    panic("never reach here");
}
void schdule(int esp, int ebp, int edi, int esi, int edx, int ecx, int ebx, int eax, int eip, int cs, int eflags)
{
    if (first_task == 1)
    {
        curtask_idx = 0;
        first_task = 0;
        task_list[curtask_idx].valid = 1;
    }
    uint32_t cur_tid = curtask_idx;
    uint32_t cur_pid = get_pid_bytid(cur_tid);
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
    {
        r0_tss.eax = task_list[0].eax;
        r0_tss.ebx = task_list[0].ebx;
        r0_tss.ecx = task_list[0].ecx;
        r0_tss.edx = task_list[0].edx;
        r0_tss.esi = task_list[0].esi;
        r0_tss.edi = task_list[0].edi;
        r0_tss.ebp = task_list[0].ebp;
        r0_tss.eip = task_list[0].eip;
        r0_tss.cs = 0x08;
        r0_tss.eflags = task_list[0].eflags | 0x200;
        r0_tss.esp = task_list[0].esp;
        r0_tss.ss = 0x10;
        r0_tss.ds = 0x18;
        r0_tss.es = 0x18;
        r0_tss.fs = 0x18;
        r0_tss.gs = 0x18;
        r0_tss.ss0 = 0x18;
        r0_tss.esp0 = task_list[0].esp;
        r0_tss.ldt = 0;
        r0_tss.trap = 0;
        r0_tss.iomap = 0;
        save_r0_tss();
    }
    uint32_t next_tid = curtask_idx;
    uint32_t next_pid = get_pid_bytid(next_tid);
    // if (0 != cur_tid && 0 != next_tid)
    if (0 == next_pid)
    {
        uint32_t cr3 = (uint32_t)kernel_pde_entry;
        asm volatile("mov %0, %%cr3" ::"r"(cr3));
    }
    else
    {
        struct PCB *pcb = get_pcb_bypid(next_pid);
        uint32_t cr3 = (uint32_t)pcb->cr3;
        asm volatile("mov %0, %%cr3" ::"r"(cr3));
        switch_r3(task_list[curtask_idx].eax,
                  task_list[curtask_idx].ebx,
                  task_list[curtask_idx].ecx,
                  task_list[curtask_idx].edx,
                  task_list[curtask_idx].edi,
                  task_list[curtask_idx].esi,
                  task_list[curtask_idx].ebp,
                  task_list[curtask_idx].eip,
                  0x20 | 3,
                  task_list[curtask_idx].eflags | (1 << 9),
                  task_list[curtask_idx].esp,
                  0x28 | 3);
        // Never return here
        panic("schdule error");
    }
    switch_task(task_list[curtask_idx].esp, task_list[curtask_idx].eip, task_list[curtask_idx].eax, task_list[curtask_idx].ebx, task_list[curtask_idx].ecx, task_list[curtask_idx].edx, task_list[curtask_idx].esi, task_list[curtask_idx].edi, task_list[curtask_idx].ebp, task_list[curtask_idx].eflags);
}
// 延迟，单位是秒
void sleep_kernel(int s)
{
    asm volatile("cli");

    task_list[curtask_idx].time_ticket = get_curtime() + s;

    asm volatile("sti");
    //转移调度权
    to_schdule();
}
// 获取可用的tid
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
// 保存r3到r0时用的ss、esp
void save_r0_tss()
{
    // get gdt table base
    uint64_t gdt_base = 0;
    // sgdt
    asm volatile("sgdt %0"
                 : "=m"(gdt_base));
    uint32_t tss_gdts = (uint32_t)((gdt_base) >> 16);
    // console_printf("gdt_base:%X\n", gdt_base >> 16);
    r0_tss_desc.limit_low = sizeof(r0_tss) - 1;
    r0_tss_desc.base_low = (uint32_t)&r0_tss & 0xffff;
    r0_tss_desc.base_mid = ((uint32_t)&r0_tss >> 16) & 0xff;
    r0_tss_desc.base_high = ((uint32_t)&r0_tss >> 24) & 0xff;
    r0_tss_desc.limit_high = 0x0;
    r0_tss_desc.type = 0x9;
    r0_tss_desc.p = 1;
    r0_tss_desc.dpl = 0;
    memcpy((void *)(tss_gdts + 8), &r0_tss_desc, sizeof(r0_tss_desc));
    // lgdt
    asm volatile("lgdt %0" ::"m"(gdt_base));
    // ltr 0x8
    asm volatile("ltr %%ax" ::"a"(0x8));
}
// 获取当前的线程id
uint32_t get_curtid()
{
    return task_list[curtask_idx].tid;
}
// 根据tid获取task结构
struct Task *get_task_bytid(uint32_t tid)
{
    for (int i = 0; i < __MAX_TASK_NUM; i++)
    {
        if (task_list[i].valid == 1)
        {
            if (task_list[i].tid == tid)
            {
                return &task_list[i];
            }
        }
    }
    panic("gettask_bytid error");
}