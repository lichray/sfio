# makefile for libstdio.a
#
# Written by Kiem-Phong Vo (4/24/91)

# Compiler and flags to use
AR=	ar
CC=	cc
CXFLAGS=
CCMODE=	-O
CCFLAGS= -I.. $(CCMODE) $(CXFLAGS)

SRCS=	doprnt.c doscan.c fclose.c fdopen.c fflush.c fgetc.c fgets.c filbuf.c \
	flsbuf.c fopen.c fprintf.c fputc.c fputs.c fread.c freopen.c fscanf.c \
	fseek.c ftell.c fwrite.c getc.c getchar.c gets.c getw.c pclose.c popen.c \
	printf.c putc.c putchar.c puts.c putw.c rewind.c scanf.c setbuf.c setbuffer.c \
	setlinebuf.c setvbuf.c sprintf.c sscanf.c stdextern.c stdstream.c \
	tmpfile.c ungetc.c vfprintf.c vfscanf.c vprintf.c vscanf.c vsprintf.c \
	vsscanf.c fileno.c feof.c ferror.c clearerr.c fpurge.c fgetpos.c fsetpos.c \
	cleanup.c vsnprintf.c snprintf.c

OBJS=	doprnt.o doscan.o fclose.o fdopen.o fflush.o fgetc.o fgets.o filbuf.o \
	flsbuf.o fopen.o fprintf.o fputc.o fputs.o fread.o freopen.o fscanf.o \
	fseek.o ftell.o fwrite.o getc.o getchar.o gets.o getw.o pclose.o popen.o \
	printf.o putc.o putchar.o puts.o putw.o rewind.o scanf.o setbuf.o setbuffer.o \
	setlinebuf.o setvbuf.o sprintf.o sscanf.o stdextern.o stdstream.o \
	tmpfile.o ungetc.o vfprintf.o vfscanf.o vprintf.o vscanf.o vsprintf.o \
	vsscanf.o fileno.o feof.o ferror.o clearerr.o fpurge.o fsetpos.o fgetpos.o \
	cleanup.o vsnprintf.o snprintf.o

.c.o:
	$(CC) -c $(CCFLAGS) $*.c

libstdio.a: must $(OBJS)
	$(AR) cr libstdio.a $(OBJS)
	-(ranlib libstdio.a; rm sfstdio.h; exit 0) >/dev/null 2>&1

must:
	sh ./sfstdhdr.sh $(CC)
	$(CC) -I.. sfstdio.c -o sfstdgen >/dev/null 2>&1
	./sfstdgen > sfstdio.h; rm sfstdgen sfstdhdr.h

clean:
	-(rm $(OBJS) sfstdgen sfstdhdr.h libstdio.a sfstdio.h; exit 0) >/dev/null 2>&1
