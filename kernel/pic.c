#include "./include/pic.h"
void init_8259()
{
    // 重新映射 IRQ 表
    // 两片级联的 Intel 8259A 芯片
    // 主片端口 0x20 0x21
    // 从片端口 0xA0 0xA1

    // 初始化主片、从片
    // 0001 0001
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // 设置主片 IRQ 从 0x20(32) 号中断开始
    outb(0x21, 0x20);
    // 设置从片 IRQ 从 0x28(40) 号中断开始
    outb(0xA1, 0x28);

    // 设置主片 IR2 引脚连接从片
    outb(0x21, 0x04);
    // 告诉从片输出引脚和主片 IR2 号相连
    outb(0xA1, 0x02);

    // 设置主片和从片按照 8086 的方式工作
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // 设置主从片允许中断
    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);
}

void pic_eoi(){
    // 发送 PIC EOI 信号
    // 发送主片信号
    outb(0x20, 0x20);
    // 发送从片信号
    outb(0xA0, 0x20);
}