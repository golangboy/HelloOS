#ifndef STDIO_H
#define STDIO_H
#include "types.h"
#include "utils.h"
void puts(char *s);
void putc(char c);
void clear();
void put_dex(uint32_t n);
void put_hex(uint32_t n);
void put_dex64(uint64_t n);
void printf(char *format, ...);
#endif