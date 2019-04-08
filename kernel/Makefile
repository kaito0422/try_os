CFLAGS :=-fno-stack-protector
#export CFLAGS

objs := display.o memory.o sheet.o window.o timer.o multitask.o

all:
	gcc -m32 -c ${CFLAGS} display.c -o display.o
	gcc -m32 -c ${CFLAGS} memory.c -o memory.o
	gcc -m32 -c ${CFLAGS} sheet.c -o sheet.o
	gcc -m32 -c ${CFLAGS} window.c -o window.o
	gcc -m32 -c ${CFLAGS} timer.c -o timer.o
	gcc -m32 -c ${CFLAGS} multitask.c -o multitask.o
	
	ld -m elf_i386 -s -r $(objs) -o kernel.o
	
.PHONY: clean
clean:
	rm -rf *.o *.bin