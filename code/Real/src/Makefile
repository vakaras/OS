SOURCES=boot.o main.o common.o gdt.o interrupt.o isr.o

# 64bit
#CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector 
#LDFLAGS=-Tlink.ld
#ASFLAGS=-felf64

# 32bit
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf
CPPFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32

fresh: clean all

all: $(SOURCES) link

clean:
	rm -f *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

.cpp.o:
	g++ $(CPPFLAGS) -c $<
