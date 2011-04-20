CLEAN = *.o kernel

update: kernel
	mv kernel ../data/supergrub-src/menus/kernel
	rm -f $(CLEAN)

clean:
	rm -f $(CLEAN)

kernel: boot.o
	../bin/x86_64-elf-ld \
		-nostdlib \
		-nodefaultlibs \
		-Tlink.ld \
		-o kernel \
		boot.o

boot.o:
	nasm -felf64 boot.s -o boot.o
