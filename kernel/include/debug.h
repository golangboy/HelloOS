#ifndef DEBUG_H
#define DEBUG_H
#include "./multiboot.h"
void print_stack_trace();
void panic(char *str);
void panic_spin(char *filename, int line, const char *func, const char *condition);
void init_sym(struct multiboot_t *m);
char *lookup_sym(uint32_t addr);
/***************************  __VA_ARGS__  *******************************
 * __VA_ARGS__ 是预处理器所支持的专用标识符。
 * 代表所有与省略号相对应的参数.
 * "..."表示定义的宏其参数可变.*/
#define PANIC(...) panic_spin(__FILE__, __LINE__, __func__, __VA_ARGS__)
/***********************************************************************/
#define ASSERT(CONDITION)                                                   \
    if (CONDITION)                                                          \
    {                                                                       \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        /* 符号#让编译器将宏的参数转化为字符串字面量 */ \
        PANIC(#CONDITION);                                                  \
    }
#endif