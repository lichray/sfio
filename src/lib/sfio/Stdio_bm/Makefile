CC=cc

all : t.sfio t.stdio t.binary

t.sfio : stdiobm.c
	$(CC) -O -I.. -I../Stdio_s stdiobm.c ../libsfio.a -o t.sfio

t.stdio : stdiobm.c
	$(CC) -O stdiobm.c -o t.stdio

t.binary : stdiobm.c
	$(CC) -O stdiobm.c ../Stdio_b/libstdio.a ../libsfio.a -o t.binary
