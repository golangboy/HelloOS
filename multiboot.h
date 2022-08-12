#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include "types.h"
struct multiboot_t
{
    uint32_t flags; // Multiboot 的版本信息
    /**
     * 从 BIOS 获知的可用内存
     *
     * mem_lower和mem_upper分别指出了低端和高端内存的大小，单位是K。
     * 低端内存的首地址是0，高端内存的首地址是1M。
     * 低端内存的最大可能值是640K。
     * 高端内存的最大可能值是最大值减去1M。但并不保证是这个值。
     */
    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device; // 指出引导程序从哪个BIOS磁盘设备载入的OS映像
    uint32_t cmdline;     // 内核命令行
    uint32_t mods_count;  // boot 模块列表
    uint32_t mods_addr;

    /**
     * ELF 格式内核映像的section头表、每项的大小、一共有几项以及作为名字索引的字符串表。
     * 它们对应于可执行可连接格式（ELF）的program头中的shdr_* 项（shdr_num等）。
     * 所有的section都会被载入，ELF section头的物理地址域指向所有的section在内存中的位置。
     */
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;

    /**
     * 以下两项指出保存由BIOS提供的内存分布的缓冲区的地址和长度
     * mmap_addr是缓冲区的地址，mmap_length是缓冲区的总大小
     * 缓冲区由一个或者多个下面的大小/结构对 mmap_entry_t（size实际上是用来跳过下一个对的）组成
     */
    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;    // 指出第一个驱动器结构的物理地址
    uint32_t drives_addr;      // 指出第一个驱动器这个结构的大小
    uint32_t config_table;     // ROM 配置表
    uint32_t boot_loader_name; // boot loader 的名字
    uint32_t apm_table;        // APM 表
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
} __attribute__((packed)) multiboot_t;
#endif