[bits 32]
extern idt_handler;
global idt_table
section .data
idt_table:
%macro idt 2
section .text
global idt_%1_entry
idt_%1_entry:
cli

%if %2==0
push 0
%endif

push %1 ;Vector

push eax
push ebx
push ecx
push edx
push esi
push edi
push ebp
push esp
call idt_handler
pop esp
pop ebp
pop edi
pop esi
pop edx
pop ecx
pop ebx
pop eax
add esp,4*2

sti
iret
section .data
dd idt_%1_entry
%endmacro
idt 0,0
idt 1,0
idt 2,0
idt 3,0
idt 4,0
idt 5,0
idt 6,0
idt 7,0
idt 8,1
idt 9,0
idt 10,1
idt 11,1
idt 12,1
idt 13,1
idt 14,1
idt 15,0
idt 16,0
idt 17,1
idt 18,0
idt 19,0
idt 20,0
idt 21,0
idt 22,0
idt 23,0
idt 24,0
idt 25,0
idt 26,0
idt 27,0
idt 28,0
idt 29,0
idt 30,0
idt 31,0
idt 32,0
idt 33,0
idt 34,0