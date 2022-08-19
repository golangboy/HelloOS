#include "timer.h"
#define IRQ0_FREQUENCY 100
#define INPUT_FREQUENCY 1193180
#define COUNTER0_VALUE INPUT_FREQUENCY / IRQ0_FREQUENCY
#define CONTRER0_PORT 0x40
#define COUNTER0_NO 0
#define COUNTER_MODE 3
#define READ_WRITE_LATCH 3
#define PIT_CONTROL_PORT 0x43
/* 把操作的计数器counter_no、读写锁属性rwl、计数器模式counter_mode写入模式控制寄存器并赋予初始值counter_value */
static void frequency_set(uint8_t counter_port,
						  uint8_t counter_no,
						  uint8_t rwl,
						  uint8_t counter_mode,
						  uint16_t counter_value)
{
	/* 往控制字寄存器端口0x43中写入控制字 */
	outb(PIT_CONTROL_PORT, (uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));
	/* 先写入counter_value的低8位 */
	outb(counter_port, (uint8_t)counter_value);
	/* 再写入counter_value的高8位 */
	outb(counter_port, (uint8_t)counter_value >> 8);
}
void init_timer()
{
	frequency_set(CONTRER0_PORT, COUNTER0_NO, READ_WRITE_LATCH, COUNTER_MODE, COUNTER0_VALUE);
}
int bcd2bin(int bcd)
{
	return ((bcd >> 4) * 10 + (bcd & 0x0f));
}
//从cmos中获取当前的时间，单位秒
uint32_t get_curtime()
{
	outb(0x70, 0x00);
	int sec = bcd2bin(inb(0x71));
	outb(0x70, 0x02);
	int min = bcd2bin(inb(0x71));
	outb(0x70, 0x04);
	int hour = bcd2bin(inb(0x71));
	outb(0x70, 0x07);
	int day = bcd2bin(inb(0x71));
	outb(0x70, 0x08);
	int month = bcd2bin(inb(0x71));
	outb(0x70, 0x09);
	int year = bcd2bin(inb(0x71)) + 30 + 1970;
	int timestamp = hour * 3600 + min * 60 + sec + day * 86400 + month * 2592000 + (year - 1970) * 31536000;
	return timestamp;
}