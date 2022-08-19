#ifndef SYSCALL_H
#define SYSCALL_H
#define __SYSCALL_TEST 0
#define __SYSCALL_SLEEP 1
#define __SYSCALL_EXITTASK 2
#define __SYSCALL_PUTS 3
int syscall(int eax, int ebx, int ecx, int edx, int edi, int esi);
#endif