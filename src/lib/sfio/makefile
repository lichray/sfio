# makefile for sfio
#
# Written by Kiem-Phong Vo (4/24/91)

INCDIR=	../../../include
LIBDIR=	../../../lib
BINDIR=	../../../bin

# Compiler and flags to use
CXFLAGS=
CCMODE=	-O
CCFLAGS= -I. $(CCMODE) $(CXFLAGS)
AR=	ar
CC=	cc

# sfio
SRCS=	sfclose.c sfclrlock.c sfcvt.c sfdisc.c sfdlen.c sfexcept.c \
	sfextern.c sffilbuf.c sfflsbuf.c sfprints.c sfgetd.c sfgetl.c \
	sfgetr.c sfgetu.c sfllen.c sfmode.c sfmove.c sfnew.c \
	sfnotify.c sfnputc.c sfopen.c sfpkrd.c sfpool.c sfpopen.c \
	sfprintf.c sfputd.c sfputl.c sfputr.c sfputu.c sfrd.c sfread.c \
	sfscanf.c sfseek.c sfset.c sfsetbuf.c sfsetfd.c sfsize.c sfsk.c \
	sfstack.c sfstrtod.c sfsync.c sftable.c sftell.c sftmp.c \
	sfungetc.c sfvprintf.c sfvscanf.c sfwr.c sfwrite.c sfexit.c \
	sfpurge.c sfpoll.c sfreserve.c sfswap.c sfraise.c
OBJS=	sfclose.o sfclrlock.o sfcvt.o sfdisc.o sfdlen.o sfexcept.o \
	sfextern.o sffilbuf.o sfflsbuf.o sfprints.o sfgetd.o sfgetl.o \
	sfgetr.o sfgetu.o sfllen.o sfmode.o sfmove.o sfnew.o \
	sfnotify.o sfnputc.o sfopen.o sfpkrd.o sfpool.o sfpopen.o \
	sfprintf.o sfputd.o sfputl.o sfputr.o sfputu.o sfrd.o sfread.o \
	sfscanf.o sfseek.o sfset.o sfsetbuf.o sfsetfd.o sfsize.o sfsk.o \
	sfstack.o sfstrtod.o sfsync.o sftable.o sftell.o sftmp.o \
	sfungetc.o sfvprintf.o sfvscanf.o sfwr.o sfwrite.o sfexit.o \
	sfpurge.o sfpoll.o sfreserve.o sfswap.o sfraise.o

# function versions of macros
SFIO_F=	Sfio_f/_sfclrerr.o Sfio_f/_sfeof.o Sfio_f/_sferror.o \
	Sfio_f/_sffileno.o Sfio_f/_sfgetc.o Sfio_f/_sfgetl.o \
	Sfio_f/_sfgetu.o Sfio_f/_sfputc.o Sfio_f/_sfslen.o \
	Sfio_f/_sfulen.o Sfio_f/_sfstacked.o Sfio_f/_sfvalue.o

# stdio-source compatibility code
STDIO_S=Stdio_s/stdgets.o Stdio_s/stdopen.o Stdio_s/stdprintf.o \
	Stdio_s/stdscanf.o Stdio_s/stdsprintf.o Stdio_s/stdvbuf.o \
	Stdio_s/stdgetc.o Stdio_s/stdputc.o

# IO disciplines
SFIO_DC=Sfio_dc/sfdcdio.o Sfio_dc/sfdcdos.o Sfio_dc/sfdcfilter.o \
	Sfio_dc/sfdclzw.o Sfio_dc/sfdcseekable.o Sfio_dc/sfdcslow.o \
	Sfio_dc/sfdcsubstream.o Sfio_dc/sfdctee.o Sfio_dc/sfdcunion.o

.c.o:
	$(CC) -c $(CCFLAGS) $*.c

all:	install

libsfio.a: ast_common.h FEATURE/sfio $(OBJS) mksfio_f mkstdio_s mksfio_dc
	$(AR) cr libsfio.a $(OBJS) $(SFIO_F) $(STDIO_S) $(SFIO_DC)
	-(ranlib libsfio.a; exit 0) >/dev/null 2>&1
FEATURE/sfio: features/sfio
	$(BINDIR)/iffe set cc $(CC) $(CCMODE) $(CXFLAGS) : run features/sfio
ast_common.h: features/common
	$(BINDIR)/iffe - set cc $(CC) $(CCMODE) $(CXFLAGS) : run features/common > ast_common.h
mksfio_f:
	cd Sfio_f; $(MAKE) -f Makefile SHELL=$(SHELL) CC="$(CC)" \
		CCMODE="$(CCMODE)" CXFLAGS="$(CXFLAGS)"
mkstdio_s:
	cd Stdio_s; $(MAKE) -f Makefile SHELL=$(SHELL) CC="$(CC)" \
		CCMODE="$(CCMODE)" CXFLAGS="$(CXFLAGS)"
mksfio_dc:
	cd Sfio_dc; $(MAKE) -f Makefile SHELL=$(SHELL) CC="$(CC)" \
		CCMODE="$(CCMODE)" CXFLAGS="$(CXFLAGS)"

libstdio.a:
	cd Stdio_b; $(MAKE) -f Makefile SHELL=$(SHELL) CC="$(CC)" \
		CCMODE="$(CCMODE)" CXFLAGS="$(CXFLAGS)"

install: libsfio.a libstdio.a
	cp sfio.h ast_common.h sfio_t.h Stdio_s/stdio.h $(INCDIR)
	cp libsfio.a Stdio_b/libstdio.a $(LIBDIR)
	-(ranlib $(LIBDIR)/libsfio.a; exit 0) >/dev/null 2>&1
	-(ranlib $(LIBDIR)/libstdio.a; exit 0) >/dev/null 2>&1

clean:
	-(rm FEATURE/* $(OBJS) ast_common.h libsfio.a; exit 0) >/dev/null 2>&1
	cd Sfio_f; $(MAKE) -f Makefile SHELL=$(SHELL) clean
	cd Stdio_s; $(MAKE) -f Makefile SHELL=$(SHELL) clean
	cd Stdio_b; $(MAKE) -f Makefile SHELL=$(SHELL) clean
	cd Sfio_dc; $(MAKE) -f Makefile SHELL=$(SHELL) clean
