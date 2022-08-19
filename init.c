#include "init.h"
#include "lib/api.h"
//第一个用户级进程
void init()
{
    for (int i = 0; i < 2; i++)
    {
        test();
        sleep(i+1+3);
    }
    exitthread();
}