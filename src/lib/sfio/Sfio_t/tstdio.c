#if !STDIO
#include	"../Stdio_s/stdio.h"
#include	"sftest.h"
#else

#include	<stdio.h>
extern int	unlink(const char*);
char*		Kpv[4] = { "/tmp/kpvaaa", "/tmp/kpvbbb", "/tmp/kpvccc", 0 };
static rmkpv()
{	int	i;
	for(i = 0; Kpv[i]; ++i)
		unlink(Kpv[i]);
}

static int	Line = -1;
#ifdef __LINE__
#define terror	Line=__LINE__,t_error
#endif
void t_error(char* s)
{	rmkpv();
	fprintf(stderr, "Line %d: %s\n", Line, s);
	exit(-1);
}
#endif

/* test compliance of certain stdio behaviors */
main()
{
#if _xopen_stdio
	FILE	*f, *f2;
	long	s1, s2;
	int	i, k, fd;
	char	buf[128*1024], rbuf[1024], *sp;

	/* test for shared streams and seek behavior */
	if(!(f = fopen(Kpv[0],"w+")) )
		terror("Opening file to read&write");

	/* change stdout to a dup of fileno(f) */
	fd = dup(1); close(1); dup(fileno(f));

	/* write something to the dup file descriptor */
	system("echo 0123456789");

	/* change stdout back */
	close(1); dup(fd); close(fd);

	/* this fseek should reset the stream back to where we can read */
	fseek(f, 0L, SEEK_SET);

	/* see if data is any good */
	fread(buf, sizeof(buf), 1, f);
	for(i = 0; i < 10; ++i)
		if(buf[i] != '0'+i)
			terror("Bad data0");

	/* construct a bunch of lines and out put to f */
	sp = buf;
	for(k = 0; k < sizeof(buf)/10; ++k)
	{	for(i = 0; i < 9; ++i)
			*sp++ = '0' + i;
		*sp++ = '\n';
	}

	/* write out a bunch of thing */
	fseek(f, 0L, SEEK_SET);
	if(fwrite(buf, sizeof(buf), 1, f) != 1)
		terror("Writing data");

	if((fd = dup(fileno(f))) < 0)
		terror("Can't dup file descriptor");
	if(!(f2 = fdopen(fd, "r")) )
		terror("Can't create stream");

	/* read a few bytes from this dup stream */
	fseek(f2, 0L, SEEK_SET);
	rbuf[0] = 0;
	if(fread(rbuf, 1, 7, f2) != 7)
		terror("Bad read\n");
	for(i = 0; i < 7; ++i)
		if(rbuf[i] != '0'+i)
			terror("Bad data1");

	if((s2 = ftell(f2)) != 7)
		terror("Bad tell location in f2");

	/* now seek way off on f */
	fseek(f, 1005L, SEEK_SET);
	rbuf[0] = 0;
	fread(rbuf, 5, 1, f);
	for(i = 5; i < 9; ++i)
		if(rbuf[i-5] != '0'+i)
			terror("Bad data2");
	if(rbuf[i-5] != '\n')
		terror("Bad data: did not get new-line");
	if((s1 = ftell(f)) != 1010)
		terror("Bad location in f");

	fseek(f, 0L, SEEK_CUR); /* switch mode so we can write */
	if(fputc('x',f) < 0)
		terror("fputc failed");
	if(fflush(f) < 0)
		terror("fflush failed");
	if((s1 = ftell(f)) != 1011)
		terror("Bad tell location in f");
	fseek(f, -1L, SEEK_CUR); /* set the seek location in the file descriptor */

	fflush(f2); /* assuming POSIX conformance and to set seek location to 1010 */
	if((s2 = ftell(f2)) != 1010)
		terror("Bad tell2 location in f2");

	fread(rbuf, 10, 1, f2);
	if(rbuf[0] != 'x')
		terror("Didn't get x");
	for(i = 1; i < 9; ++i)
		if(rbuf[i] != '0'+i)
			terror("Bad data3");
	if(rbuf[i] != '\n')
		terror("Did not get new-line");

	rmkpv();
#endif

	return 0;
}
