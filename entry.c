int entry()
{
    char *vga = 0xb8000;
    char *ptr = "Hello Kernel!";
    for (int i = 0; *ptr; i += 2, ptr++)
    {
        vga[i] = *ptr;
    }
    while (1)
        ;
    return 0;
}