# makefile for discipline library
#
# Written by Kiem-Phong Vo (6/18/93)

INCDIR=	../../../include
LIBDIR=	../../../lib

# Native compiler if building libsfdisc.a for a cross compilation environment
NC=	cc

# Compiler and flags to use
CXFLAGS=
CCMODE=	-O
CCFLAGS= -I. -I$(INCDIR) $(CCMODE) $(CXFLAGS)
AR=	ar
CC=	cc

# sfdisc
SRCS=	sfdcfilter.c sfdclzw.c sfdcskable.c sfdcsubstream.c sfdctee.c sfdcunion.c
OBJS=	sfdcfilter.o sfdclzw.o sfdcskable.o sfdcsubstream.o sfdctee.o sfdcunion.o

.c.o:
	$(CC) -c $(CCFLAGS) $*.c

all:	install

libsfdisc.a: $(OBJS)
	$(AR) cr libsfdisc.a $(OBJS)
	-(ranlib libsfdisc.a; exit 0) >/dev/null 2>&1

install: libsfdisc.a
	cp sfdisc.h $(INCDIR)
	cp libsfdisc.a $(LIBDIR)
	-(ranlib $(LIBDIR)/libsfdisc.a; exit 0) >/dev/null 2>&1

clean:
	-(rm $(OBJS) libsfdisc.a; exit 0) >/dev/null 2>&1
