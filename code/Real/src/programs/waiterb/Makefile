CLEAN = *.o *~ program

SOURCES = waiter.o
LDFLAGS = -nostdlib -nodefaultlibs -Tlink.ld
ASFLAGS = -felf64

update: clean program
	mv program ../../../data/supergrub-src/menus/waiterb
	rm -f $(CLEAN)

clean:
	rm -f $(CLEAN)

program: $(SOURCES)
	ld $(LDFLAGS) -o program $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
