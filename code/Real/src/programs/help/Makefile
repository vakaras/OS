CLEAN = *.o *~ program

SOURCES = icxxabi.o program.o load.o stdio.o
LDFLAGS = -nostdlib -nodefaultlibs -Tlink.ld
ASFLAGS = -felf64
CPPFLAGS = -m64 -mcmodel=large \
					 -Wall -Wextra -Werror \
					 -nostdlib -fno-builtin -nostartfiles -nodefaultlibs \
					 -fno-exceptions -fno-rtti -fno-stack-protector \
					 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 \
					 -mno-3dnow 

update: clean program
	mv program ../../../data/supergrub-src/menus/help
	rm -f $(CLEAN)

clean:
	rm -f $(CLEAN)

program: $(SOURCES)
	ld $(LDFLAGS) -o program $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

.cpp.o:
	../../../bin/x86_64-elf-g++ $(CPPFLAGS) -c $<
