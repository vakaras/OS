CLEAN = *.o *~ program

SOURCES = program.o
LDFLAGS = -nostdlib -nodefaultlibs -Tlink.ld
ASFLAGS = -felf64

update: clean program
	mv program ../../../data/supergrub-src/menus/writer
	rm -f $(CLEAN)

clean:
	rm -f $(CLEAN)

program: $(SOURCES)
	ld $(LDFLAGS) -o program $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
