#include "strings.h"
int strcmp(char *str1, char *str2)
{
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] != str2[i])
        {
            return str1[i] - str2[i];
        }
        i++;
    }
    if (str1[i] == '\0' && str2[i] == '\0')
    {
        return 0;
    }
    if (str1[i] == '\0')
    {
        return -1;
    }
    return 1;
}
int strcpy(char *str1, char *str2)
{
    int i = 0;
    while (str2[i] != '\0')
    {
        str1[i] = str2[i];
        i++;
    }
    str1[i] = '\0';
    return 0;
}
int memcpy(void *dst, void *src, int len)
{
    int i = 0;
    while (i < len)
    {
        ((char *)dst)[i] = ((char *)src)[i];
        i++;
    }
    return 0;
}