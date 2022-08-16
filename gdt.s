[GLOBAL reload_gdt]
global reload_gdt
section .text
reload_gdt:
    ;lgdt [gdt_ptr+0xc0000000]
    sgdt [gdt_ptr]
    mov ebx , [gdt_ptr+0x2]
    add dword  [gdt_ptr+0x2] ,0xc0000000
    lgdt [gdt_ptr]
    ret

section .data
gdt_ptr:
    dw 5*8-1
    dd gdt_table
gdt_table:
    dq 0
    dq 0
    dq 0xCF9A000000FFFF
    dq 0xCF92000000FFFF
    dq 0