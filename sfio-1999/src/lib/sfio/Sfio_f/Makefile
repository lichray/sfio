# makefile for stdio-source compatibility code
#
# Written by Kiem-Phong Vo (4/24/91)

# Compiler and flags to use
AR=	ar
CC=	cc
CXFLAGS=
CCMODE=	-O
CCFLAGS= -I.. $(CCMODE) $(CXFLAGS)

SRCS=	_sfclrerr.c _sfdlen.c _sfeof.c _sferror.c _sffileno.c _sfgetc.c \
	_sfgetl.c _sfgetu.c _sfllen.c _sfputc.c _sfputd.c _sfputl.c _sfputu.c \
	_sfslen.c _sfstacked.c _sfulen.c _sfvalue.c
OBJS=	_sfclrerr.o _sfdlen.o _sfeof.o _sferror.o _sffileno.o _sfgetc.o \
	_sfgetl.o _sfgetu.o _sfllen.o _sfputc.o _sfputd.o _sfputl.o _sfputu.o \
	_sfslen.o _sfstacked.o _sfulen.o _sfvalue.o

.c.o:
	$(CC) -c $(CCFLAGS) $*.c

must:	$(OBJS)

clean:
	-(rm $(OBJS); exit 0) >/dev/null 2>&1
