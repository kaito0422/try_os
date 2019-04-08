CFLAGS :=-fno-stack-protector
#export CFLAGS

all : kernel/kernel.o
	make -C boot all
	dd if=./boot/boot.bin of=a.img bs=512 count=1 conv=notrunc
	
	nasm -f elf32 elf.asm -o elf.o
	gcc -m32 -c ${CFLAGS} start.c -o start.o
	gcc -m32 -c ${CFLAGS} main.c -o main.o
	
	ld -m elf_i386 -Ttext 0x50000 -s elf.o start.o main.o ./kernel/kernel.o -o elf.bin
	
	mount -o loop a.img /mnt/floppy
	cp ./boot/setup.bin /mnt/floppy -v
	cp elf.bin /mnt/floppy -v
	umount /mnt/floppy

kernel/kernel.o	:
	make -C kernel all
	
.PHONY: clean
clean:
	rm -rf *.o *.bin
	make -C boot clean
	make -C kernel clean