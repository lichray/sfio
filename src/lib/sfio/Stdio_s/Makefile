# makefile for stdio-source compatibility code
#
# Written by Kiem-Phong Vo (4/24/91)

# Compiler and flags to use
AR=	ar
CC=	cc
CXFLAGS=
CCMODE=	-O
CCFLAGS= -I.. $(CCMODE) $(CXFLAGS)

SRCS=	stdgets.c stdprintf.c stdscanf.c stdvbuf.c stdsprnt.c \
	stdvsprnt.c stdvsnprnt.c stdvsscn.c stdopen.c
OBJS=	stdgets.o stdprintf.o stdscanf.o stdvbuf.o stdsprnt.o \
	stdvsprnt.o stdvsnprnt.o stdvsscn.o stdopen.o

.c.o:
	$(CC) -c $(CCFLAGS) $*.c

must:	$(OBJS)

clean:
	-(rm $(OBJS); exit 0) >/dev/null 2>&1
