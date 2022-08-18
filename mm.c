#include "mm.h"
#include "console.h"
#include "multiboot.h"
#include "debug.h"
int mg_bkcnt = 0;
struct MEM_MG MEM_MG;
extern uint8_t kern_start[];
extern uint8_t kern_end[];
extern void reload_gdt();
// struct PDE kernelPde[256];
// uint32_t cr3entry[1024];
/**
 * size是相关结构的大小，单位是字节，它可能大于最小值20
 * base_addr_low是启动地址的低32位，base_addr_high是高32位，启动地址总共有64位
 * length_low是内存区域大小的低32位，length_high是内存区域大小的高32位，总共是64位
 * type是相应地址区间的类型，1代表可用RAM，所有其它的值代表保留区域
 */
typedef struct mmap_entry_t
{
    uint32_t size; // 留意 size 是不含 size 自身变量的大小
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;

void print_memory_map(struct multiboot_t *m)
{
    uint32_t mmap_addr = m->mmap_addr;
    uint32_t mmap_length = m->mmap_length;
    console_printf("Kernel_start: %x Kernel_end: %x size:%x KB\n", (int)kern_start, (int)kern_end, ((int)kern_end - (int)kern_start) / 1024);
    console_write_color("Memory map:\n", rc_black, rc_red);

    mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
    for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++)
    {
        uint64_t start = ((long long)mmap->base_addr_high << 32) | mmap->base_addr_low;
        uint64_t size = ((long long)mmap->length_high << 32) | mmap->length_low;
        if (start >= 0x100000 && mmap->type == 1)
        {
            if (start == 0x100000)
            {
                uint64_t kenerl_size = ((int)kern_end - (int)kern_start);
                add_memmg(start + kenerl_size, (uint64_t)(size - kenerl_size));
            }
            else
            {
                add_memmg(start, size);
            }
        }
        console_printf("base_addr = %X, length = %X type:%d \n", start, size, mmap->type);
    }
}
void add_memmg(uint64_t addr, uint64_t size)
{
    // console_printf("Add memory: %x %x\n", addr, size);
    MEM_MG.freemem[mg_bkcnt].pid = 0;
    MEM_MG.freemem[mg_bkcnt].start_addr = addr;
    MEM_MG.freemem[mg_bkcnt++].size = size;
}
void init_vm()
{
    struct PDE *pde_entry = (struct PDE *)alloc_4k(1024 * 4);
    kernel_pde_entry = pde_entry;
    ASSERT(pde_entry != 0);
    ASSERT(((int)pde_entry & (0x1000 - 1)) == 0);
    for (int i = 0; i < 1024; i++)
    {
        pde_entry[i].present = 0;
    }
    pde_entry[0].present = 1;
    pde_entry[0].frame = ((int)alloc_4k(1024 * 4)) >> 12;
    for (int i = 0, b = 0; i < 1024; i++)
    {
        struct PTE *pte_entry = (pde_entry[0].frame) << 12;
        pte_entry[i].present = 1;
        pte_entry[i].frame = b >> 12;
        b += 4096;
    }
    for (int i = 0x300, b = 0; i < 1024; i++)
    {
        struct PTE *pte_entry = (struct PTE *)alloc_4k(1024 * 4);
        ASSERT(((int)pte_entry & (0x1000 - 1)) == 0);
        for (int j = 0; j < 1024; j++)
        {
            pte_entry[j].present = 1;
            pte_entry[j].frame = b >> 12;
            b += 4096;
        }
        pde_entry[i].frame = ((int)pte_entry) >> 12;
        pde_entry[i].present = 1;
    }
    asm volatile("mov %0, %%cr3" ::"r"(pde_entry));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0"
                 : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" ::"r"(cr0));
    reload_gdt();
}
void merge()
{
    // sort by size
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        for (int j = 0; j < __MAX_MEMBK_CNT - 1; j++)
        {
            if (MEM_MG.freemem[j].start_addr > MEM_MG.freemem[j + 1].start_addr)
            {
                struct MEM_BLOCK temp = MEM_MG.freemem[j];
                MEM_MG.freemem[j] = MEM_MG.freemem[j + 1];
                MEM_MG.freemem[j + 1] = temp;
            }
        }
    }
    // merge
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        if (MEM_MG.freemem[i].size == 0)
        {
            continue;
        }
        for (int j = i + 1; j < __MAX_MEMBK_CNT; j++)
        {
            if (MEM_MG.freemem[j].size == 0)
            {
                continue;
            }
            if (MEM_MG.freemem[i].start_addr + MEM_MG.freemem[i].size == MEM_MG.freemem[j].start_addr)
            {
                MEM_MG.freemem[i].size += MEM_MG.freemem[j].size;
                MEM_MG.freemem[j].size = 0;
            }
        }
    }
}
int free(void *ptr)
{
    uint64_t addr = (uint64_t)ptr;
    uint64_t size = 0;
    if (0 == addr)
    {
        return 0;
    }
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        if (MEM_MG.allocmem[i].start_addr == addr)
        {
            size = MEM_MG.allocmem[i].size;
            MEM_MG.allocmem[i].start_addr = 0;
            MEM_MG.allocmem[i].size = 0;
            break;
        }
    }
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        if (MEM_MG.freemem[i].start_addr == addr + size)
        {
            MEM_MG.freemem[i].start_addr -= size;
            MEM_MG.freemem[i].size += size;
            return 1;
        }
    }
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        if (MEM_MG.freemem[i].size == 0)
        {
            MEM_MG.freemem[i].start_addr = addr;
            MEM_MG.freemem[i].size = size;
            return 1;
        }
    }
    return 0;
}

void *alloc(uint64_t size)
{
    merge();
    if (0 == size)
    {
        return 0;
    }
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        if (MEM_MG.freemem[i].size >= size)
        {
            int addr = MEM_MG.freemem[i].start_addr;
            MEM_MG.freemem[i].start_addr += size;
            MEM_MG.freemem[i].size -= size;
            for (int j = 0; j < __MAX_MEMBK_CNT; j++)
            {
                if (MEM_MG.allocmem[j].size == 0)
                {
                    MEM_MG.allocmem[j].start_addr = addr;
                    MEM_MG.allocmem[j].size = size;
                    break;
                }
            }
            return (void *)addr;
        }
    }
    return 0;
}
void *alloc_4k(uint64_t size)
{
    merge();
    if (0 == size)
    {
        return 0;
    }
    int idx = -1;
    uint64_t alloc_addr = 0;
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        if (MEM_MG.freemem[i].size >= size)
        {
            for (uint64_t k = MEM_MG.freemem[i].start_addr & 0xfffff000;
                 (k) < (MEM_MG.freemem[i].start_addr + MEM_MG.freemem[i].size); k += 4096)
            {
                if ((k >= MEM_MG.freemem[i].start_addr) &&
                    (k + size < MEM_MG.freemem[i].start_addr + MEM_MG.freemem[i].size))
                {
                    alloc_addr = k;
                    break;
                }
            }
            if (alloc_addr)
            {
                idx = i;
                break;
            }
        }
    }
    if (-1 == idx)
    {
        return 0;
    }
    uint64_t free_size = alloc_addr - MEM_MG.freemem[idx].start_addr;
    uint64_t old_start_addr = MEM_MG.freemem[idx].start_addr;
    MEM_MG.freemem[idx].start_addr += (size + free_size);
    MEM_MG.freemem[idx].size -= (size + free_size);
    if (free_size)
    {
        for (int i = 0; i < __MAX_MEMBK_CNT; i++)
        {
            if (MEM_MG.freemem[i].size == 0)
            {
                MEM_MG.freemem[i].start_addr = old_start_addr;
                MEM_MG.freemem[i].size = free_size;
                break;
            }
        }
    }
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        if (MEM_MG.allocmem[i].size == 0)
        {
            MEM_MG.allocmem[i].start_addr = alloc_addr;
            MEM_MG.allocmem[i].size = size;

            break;
        }
    }
    return (void *)alloc_addr;
}
uint64_t get_freemem()
{
    uint64_t size = 0;
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        size += MEM_MG.freemem[i].size;
    }
    return size;
}

uint64_t get_allocmem()
{
    uint64_t size = 0;
    for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    {
        size += MEM_MG.allocmem[i].size;
    }
    return size;
}

void mg_info()
{
    console_write_color("Memory Manage:\n", rc_black, rc_red);
    console_printf("Allocated memory: %D KB\n", (uint64_t)(get_allocmem() / 1024));
    // for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    // {
    //     if (MEM_MG.allocmem[i].start_addr != 0)
    //     {
    //         console_printf("  [ %d ]- Start:%x Size:%x\n", i, MEM_MG.allocmem[i].start_addr, MEM_MG.allocmem[i].size);
    //     }
    // }
    console_printf("Free memory: %D KB\n", (uint64_t)(get_freemem() / 1024));
    // for (int i = 0; i < __MAX_MEMBK_CNT; i++)
    // {
    //     if (MEM_MG.freemem[i].start_addr != 0)
    //     {
    //         console_printf("  [ %d ] - Start:%x Size:%x\n", i, MEM_MG.freemem[i].start_addr, MEM_MG.freemem[i].size);
    //     }
    // }
}