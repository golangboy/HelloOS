[bits 32]
[GLOBAL switch_task]
global switch_task
;int esp, int eip,int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp, int eflags
switch_task:
    add esp,4        ;remove ret address
    mov eax, [esp]   ;target esp
    mov ebx, [esp+4] ;target eip
    mov ecx, [esp+8] ;target eax
    mov [eax-4], ebx ;save eip
    mov [eax-8], ecx ;save eax
    add esp,12       ;remove args
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    pop ebp
    popfd
    mov esp, eax     ;set esp to target esp
    sub esp,8
    pop eax
    sti
    ret