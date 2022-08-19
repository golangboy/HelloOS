#ifndef IDT_H
#define IDT_H
#define __IDT_NUM 50
#define __SYSTEM_CALL_IDTVEC 49
#define __TIMER_IDTVEC 32
#define __KEYBOARD_IDTVEC 33
#define __PAGE_FAULT_IDTVEC 14
void load_idt();
#endif