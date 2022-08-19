#include "./include/init.h"
#include "./include/stdio.h"
#include "./include/proc.h"
void second();
//第一个用户级进程
void init()
{
    clear();
    createthread(second, 0x500000);
    for (int i = 0; i < 5; i++)
    {
        // printf("Task A:%d\n", i);
        sleep(i + 1 + 3);
    }
    printf("Exit Task A:%d\n");
    exitthread();
}
void second()
{
    printf("Hello Second Process\n");
    sleep(3);
    printf("ByByte Second Process\n");
    for (int i = 0; i < 5; i++)
    {
        printf("Task B:%d", i);
        sleep(i + 1 + 3);
        printf("\b \b");
        sleep(2);
        printf("\n");
    }
    exitthread();
}