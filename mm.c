#include "mm.h"
#include "console.h"
#include "multiboot.h"
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

    console_write_color("Memory map:\n", rc_black, rc_red);

    mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
    for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++)
    {
        long long start = ((long long)mmap->base_addr_high << 32) | mmap->base_addr_low;
        long long size = ((long long)mmap->length_high << 32) | mmap->length_low;
        console_printf("base_addr = %X, length = %X type:%d \n", start, size, mmap->type);
    }
}