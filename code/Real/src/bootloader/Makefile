# This file content was copied from JohnnyTheDon post at 
# http://f.osdev.org/viewtopic.php?f=1&t=17548

CC = gcc
CFLAGS = -m32 -std=gnu99 -nostartfiles -nostdlibs -nodefaultlibs

ASM = yasm
ASMFLAGS = -f elf32

LD = ld
LDFLAGS = -melf_i386 -T multiboot-elf32.ld -nostdlibs -nodefaultlibs

OUTPUT = bootloader

.PHONY : all clean

all: $(OUTPUT)
clean: 
	rm -f main.o 
	rm -f main16.bin
	rm -f c.o
$(OUTPUT): main.o c.o 
	$(LD) $(LDFLAGS) -o $(OUTPUT) main.o c.o
main.o : main.s main16.bin 
	$(ASM) $(ASMFLAGS) -o main.o main.s
c.o : c.c 
	$(CC) $(CFLAGS) -c -o c.o c.c
main16.bin : main16.s 
	$(ASM) -f bin -o main16.bin main16.s
main16.s :
main.s :
c.c:
