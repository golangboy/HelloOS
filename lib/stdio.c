#include "stdio.h"

void puts(char *s)
{
    asm volatile("mov %0, %%ebx"
                 :
                 : "r"(s));
    asm volatile("mov $3, %eax");
    asm volatile("int $49");
}
void clear()
{
    asm volatile("mov $4, %eax");
    asm volatile("int $49");
}
void putc(char c)
{
    asm volatile("mov %0, %%ebx"
                 :
                 : "r"((int)c));
    asm volatile("mov $5, %eax");
    asm volatile("int $49");
}
void put_dex(uint32_t n)
{
    if (n == 0)
    {
        putc('0');
        return;
    }

    uint32_t acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc % 10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;

    int j = 0;
    while (i >= 0)
    {
        c2[i--] = c[j++];
    }

    puts(c2);
}
void put_hex(uint32_t n)
{
    int tmp;
    char noZeroes = 1;

    puts("0x");
    if (n == 0)
    {
        puts("0");
        return;
    }

    int i;
    for (i = 28; i >= 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
        noZeroes = 0;
        if (tmp >= 0xA)
        {
            putc(tmp - 0xA + 'a');
        }
        else
        {
            putc(tmp + '0');
        }
    }
}
void put_dex64(uint64_t n)
{
    if (n == 0)
    {
        putc('0');
        return;
    }

    uint64_t acc = n;
    char c[64];
    int i = 0;
    while (acc > 0)
    {
        int ys = do_div(acc, 10);
        c[i] = '0' + ys;
        i++;
    }
    c[i] = 0;

    char c2[64];
    c2[i--] = 0;

    int j = 0;
    while (i >= 0)
    {
        c2[i--] = c[j++];
    }
    puts(c2);
}
void put_hex64(uint64_t n)
{
    int tmp;
    char noZeroes = 1;

    puts("0x");
    if (n == 0)
    {
        puts("0");
        return;
    }

    int i;
    for (i = 60; i >= 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
        noZeroes = 0;
        if (tmp >= 0xA)
        {
            putc(tmp - 0xA + 'a');
        }
        else
        {
            putc(tmp + '0');
        }
    }
}
void printf(char *format, ...)
{
    char *arg = ((char *)((&format)));
    arg += 4;
    char *cstr = format;
    while (*cstr)
    {
        if (*cstr == '%')
        {
            cstr++;
            switch (*cstr)
            {
            case 'c':
                putc(*arg);
                arg += 4;
                break;
            case 's':
                puts((char *)*((int *)arg));
                arg += 4;
                break;
            case 'd':
                put_dex(*((int *)arg));
                arg += 4;
                break;
            case 'D':
                put_dex64(*((long long *)arg));
                arg += 8;
                break;
            case 'x':
                put_hex(*((int *)arg));
                arg += 4;
                break;
            case 'X':
                put_hex(*((long long *)arg));
                arg += 8;
                break;
            default:
                putc('%');
                putc(*cstr);
                break;
            }
        }
        else
        {
            putc(*cstr);
        }
        cstr++;
    }
}