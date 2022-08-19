#ifndef PROC_H
#define PROC_H
#include "../../lib/include/types.h"
void sleep(uint32_t s);
void exitthread();
uint32_t createthread(void *entry, uint32_t esp);
#endif