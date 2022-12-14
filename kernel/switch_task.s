[bits 32]
[GLOBAL switch_task]
[GLOBAL switch_r3]
[GLOBAL to_schdule]
global switch_task
global switch_r3
global to_schdule
extern before_schdule
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
switch_r3:
    add esp,4        ;remove ret address
    mov eax,0x28|3
    mov ds,eax
    mov fs,eax
    mov gs,eax
    mov es,eax
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop edi
    pop esi
    pop ebp
    iret
to_schdule:
    ;保存当前的寄存器上下文，用于交给调度器处理
    ;eip
    push esp
    push ebp
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push cs
    pushfd
    call before_schdule