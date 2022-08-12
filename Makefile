all:
	nasm -f elf -g -F stabs boot.s -o bin/boot.o
	nasm -f elf -g -F stabs interrupt.s -o bin/interrupt.o
	cd bin && gcc -c -m32 -g  -gstabs+ -nostdinc -fno-builtin -fno-stack-protector ../*.c
	ld -T kernel.ld -m elf_i386 -nostdlib bin/*.o -o bin/kernel
move:
	rm -r -f /g
	mkdir /g
	losetup -P /dev/loop10  hd
	mount /dev/loop10p1 /g
	cp bin/kernel /g/kernel
	umount /g
	losetup -d /dev/loop10
clean:
	rm ./bin/*
debug:
	qemu-system-i386.exe -S -s -hda .\hd
run:
	qemu-system-i386.exe -hda .\hd