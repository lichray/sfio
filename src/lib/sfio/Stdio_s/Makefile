# makefile for stdio-source compatibility code
#
# Written by Kiem-Phong Vo (4/24/91)

# Compiler and flags to use
AR=	ar
CC=	cc
CXFLAGS=
CCMODE=	-O
CCFLAGS= -I.. $(CCMODE) $(CXFLAGS)

SRCS=	stdgets.c stdopen.c stdprintf.c stdscanf.c stdsprintf.c stdvbuf.c \
	stdputc.c stdgetc.c
OBJS=	stdgets.o stdopen.o stdprintf.o stdscanf.o stdsprintf.o stdvbuf.o \
	stdputc.o stdgetc.o

.c.o:
	$(CC) -c $(CCFLAGS) $*.c

must:	$(OBJS)

clean:
	-(rm $(OBJS); exit 0) >/dev/null 2>&1
