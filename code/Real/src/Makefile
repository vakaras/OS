SOURCES=boot.o main.o

# 64bit
#CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector 
#LDFLAGS=-Tlink.ld
#ASFLAGS=-felf64

# 32bit
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf
CPPFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32

all: $(SOURCES) link

clean:
	rm *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

.cpp.o:
	g++ $(CPPFLAGS) -c $<
