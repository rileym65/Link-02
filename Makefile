PROJECT=link02
CC=gcc
DEFS=
INDIR=
INCS=
LIBDIR=
LIBS=
OBJS=\
	addlibrary.o \
	addobject.o \
	addreference.o \
	dolink.o \
	findsymbol.o \
	gethex.o \
	loadfile.o \
	main.o \
	outputbinary.o \
	outputelfos.o \
	outputintel.o \
	outputrcs.o \
	readcontrolfile.o \
	readmem.o \
	sortsymbols.o \
	writemem.o

$(PROJECT): $(OBJS)
	$(CC) $(DEFS) $(LIBDIR) $(OBJS) $(LIBS) -o $(PROJECT)

.c.o:
	$(CC) $(DEFS) $(INCDIR) $(INCS) -c $<

clean:
	-rm *.o
	-rm $(PROJECT)

install:
	cp link02 /usr/local/bin
	chmod a+rx /usr/local/bin/link02

addlibrary.o:          header.h addlibrary.c
addobject.o:           header.h addobject.c
addreference.o:        header.h addreference.c
dolink.o:              header.h dolink.c
findsymbol.o:          header.h findsymbol.c
gethex.o:              header.h gethex.c
loadfile.o:            header.h loadfile.c
main.o:                header.h main.c
outputbinary.o:        header.h outputbinary.c
outputelfos.o:         header.h outputelfos.c
outputintel.o:         header.h outputintel.c
outputrcs.o:           header.h outputrcs.c
readcontrolfile.o:     header.h readcontrolfile.c
readmem.o:             header.h readmem.c
sortsymbols.o:         header.h sortsymbols.c
writemem.o:            header.h writemem.c

