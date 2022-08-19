#include "idt.h"
#include "console.h"
#include "elf.h"
#include "syscall.h"
#include "task.h"
#include "mm.h"
extern int idt_table;
typedef struct __attribute__((packed)) interupt_gate
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t offset_high;
} interupt_gate;
typedef struct __attribute__((packed)) interupt_descriptor
{
    uint16_t limit;
    uint32_t base;
    struct interupt_gate idt[__IDT_NUM];
} interupt_descriptor;
struct interupt_descriptor idt_desc;
void load_idt()
{
    // console_printf("Loading IDT\n");
    for (int i = 0; i <= (__IDT_NUM - 1); i++)
    {
        int base = *((&idt_table) + i);
        // console_printf("IDT Vector:%d - Handler:%x\n", i, base);
        idt_desc.idt[i].offset_low = base & 0xFFFF;
        idt_desc.idt[i].offset_high = (base >> 16) & 0xFFFF;
        idt_desc.idt[i].selector = 0x10;
        idt_desc.idt[i].always0 = 0;
        idt_desc.idt[i].flags = 0x8E;
        if (__SYSTEM_CALL_IDTVEC == i)
        {
            idt_desc.idt[i].flags = 0xEE;
        }
        else if (__TIMER_IDTVEC == i)
        {
            idt_desc.idt[i].flags = 0xEE;
        }
    }
    idt_desc.base = (uint32_t) & (idt_desc.idt);
    idt_desc.limit = (sizeof(struct interupt_gate) * __IDT_NUM) - 1;
    asm volatile("lidt %0"
                 :
                 : "m"(idt_desc));
}
void idt_handler(int esp, int ebp, int edi, int esi, int edx, int ecx, int ebx, int eax, int vecNum, int errCode, int eip, int cs, int eflags)
{
    if (39 == vecNum)
    {
        return;
    }
    // esp+0x30 is the stack pointer
    // console_printf("IDT Vector:%d - EIP:%x - Name:%s \n", vecNum, eip, lookup_sym(eip));
    // console_printf("Error Code:%x\n", errCode);
    // console_printf("EIP:%x\n", eip);
    // console_printf("CS:%x\n", cs);
    // console_printf("EFLAGS:%x\n", eflags);
    // console_printf("EAX:%x EBX:%x ECX:%x EDX:%x\n EDI:%x ESI:%x ESP:%x EBP:%x\n", eax, ebx, ecx, edx, edi, esi, esp, ebp);
    if (vecNum == __TIMER_IDTVEC)
    {
        int src_cs = 0;
        // read cs register
        asm volatile("mov %%cs, %0"
                     : "=r"(src_cs));
        int tmp = eflags;
        if (eflags & 0x200)
        {
            tmp = eflags ^ 0x200;
        }
        if ((src_cs & 3) != (cs & 3)) // 特权级发生变化
        {
            int r3_eip = 0;
            int r3_cs = 0;
            int r3_eflags = 0;
            int r3_esp = 0;
            int r3_ss = 0;

            int src_esp = (int)((int *)esp + 9);
            r3_eip = *((int *)src_esp);
            r3_cs = *((int *)src_esp + 1);
            r3_eflags = *((int *)src_esp + 2);
            r3_esp = *((int *)src_esp + 3);
            r3_ss = *((int *)src_esp + 4);
            if (r3_eflags & 0x200)
            {
                r3_eflags = r3_eflags ^ 0x200;
            }
            schdule(r3_esp, ebp, edi, esi, edx, ecx, ebx, eax, r3_eip, r3_cs, r3_eflags);
        }
        else
        {
            schdule(esp + 0x30, ebp, edi, esi, edx, ecx, ebx, eax, eip, cs, tmp);
        }

        // console_printf("Nerver reach here\n");
    }
    else if (vecNum == __SYSTEM_CALL_IDTVEC) // 系统调用
    {
        // console_printf("[System Call Test]\n");
        uint32_t ret = syscall(eax, ebx, ecx, edx, edi, esi);
        // change eax
        asm volatile("mov %0, %%eax"
                     :
                     : "r"(ret));
        return;
    }
    else if (vecNum == __PAGE_FAULT_IDTVEC)
    {
        console_printf("IDT Vector:%d - EIP:%x - Name:%s\n", vecNum, eip, lookup_sym(eip));
        page_falut();
    }
    else
    {
        // console_printf("IDT Vector:%d - EIP:%x - Name:%s \n", vecNum, eip, lookup_sym(eip));
    }
}