#include "init.h"
#include "api.h"
void init()
{
    for (int i = 0; i < 2; i++)
    {
        test();
        sleep(i+1+3);
    }
    exitthread();
}