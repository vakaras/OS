CLEAN = *.o *~ kernel

SOURCES = boot.o main.o icxxabi.o primitives.o \
					isr.o paging.o memlib.o debug.o debug.asm.o gdt.o primitives.asm.o
LDFLAGS = -nostdlib -nodefaultlibs -Tlink.ld
ASFLAGS = -felf64
CPPFLAGS = -m64 -mcmodel=large \
					 -Wall -Wextra -Werror \
					 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs \
					 -fno-exceptions -fno-rtti -fno-stack-protector \
					 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 \
					 -mno-3dnow 
					 

update: clean kernel
	mv kernel ../data/supergrub-src/menus/kernel
	rm -f $(CLEAN)

clean:
	rm -f $(CLEAN)

kernel: $(SOURCES)
	../bin/x86_64-elf-ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

.cpp.o:
	../bin/x86_64-elf-g++ $(CPPFLAGS) -c $<

