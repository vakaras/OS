CLEAN = *.o kernel

SOURCES = boot.o main.o
LDFLAGS = -nostdlib -nodefaultlibs -Tlink.ld
ASFLAGS = -felf64
CPPFLAGS = -nostdlib -nostdinc -fno-builtin -fno-stack-protector

update: kernel
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

