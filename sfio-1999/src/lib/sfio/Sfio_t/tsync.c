#include	"sftest.h"

static int	Didsync;

#if __STD_C
static except(Sfio_t* f, int type, Void_t* data, Sfdisc_t* disc)
#else
static except(f, type, data, disc)
Sfio_t*		f;
int		type;
Void_t*		data;
Sfdisc_t*	disc;
#endif
{
	if(type == SF_SYNC)
		Didsync = 1;
	return 0;
}

Sfdisc_t	Disc = { NIL(Sfread_f), NIL(Sfwrite_f), NIL(Sfseek_f), except };

main()
{
	int	p1[2], p2[2];
	int	n, dupf2;
	Sfoff_t	s;
	char	buf[1024];
	Sfio_t	*f1, *f2;

	/* redirect stdout to a pipe */
	if(pipe(p1) < 0 || close(1) < 0 || dup(p1[1]) != 1)
		terror("Creating pipe1\n");

	/* redirect stdin to a pipe with some input */
	close(0);
	if(pipe(p2) < 0)
		terror("Creating a pipe2\n");
	if(write(p2[1],"foo\n",4) != 4)
		terror("Writing to pipe\n");
	close(p2[1]);

	sfsetbuf(sfstdout,buf,sizeof(buf));
	if(sfset(sfstdout,0,0)&SF_LINE)
		terror("Line mode on unexpectedly\n");
	sfset(sfstdout,SF_SHARE,0);

	if(sfeof(sfstdin) )
		terror("Premature eof\n");
	if(sferror(sfstdout) )
		terror("Weird error1\n");
	if((s = sfmove(sfstdin,sfstdout,(Sfoff_t)SF_UNBOUND,-1)) != 4)
		terror("Wrong # of bytes %lld\n", s);
	if(!sfeof(sfstdin) )
		terror("Should be eof\n");
	if(sferror(sfstdout) )
		terror("Weird error2\n");
	if(sfpurge(sfstdout) < 0)
		terror("Purging stdout\n");

	if(!(f1 = sfopen(NIL(Sfio_t*), Kpv[0], "w")) )
		terror("Opening file to write\n");
	if(!(f2 = sfopen(NIL(Sfio_t*), Kpv[0],"r")) )
		terror("Opening file to read\n");

	sfset(f1,SF_IOCHECK,1);
	sfdisc(f1,&Disc);
	sfsetbuf(f1,NIL(char*),4);
	sfputc(f1,'1');
	sfputc(f1,'2');
	sfputc(f1,'3');
	sfputc(f1,'4');
	sfputc(f1,'5');
	sfputc(f1,'6');
	if((n = sfread(f2,buf,sizeof(buf))) != 4)
		terror("Did not get all data\n");
	if(Didsync)
		terror("Should not have seen SF_SYNC yet\n");
	sfsync(NIL(Sfio_t*));
	if(!Didsync)
		terror("Should have seen SF_SYNC\n");

	sfputc(f1,'7');
	sfputc(f1,'8');

	Didsync = 0;
	sfsync(f1);
	if(!Didsync)
		terror("Did not see SF_SYNC1\n");
	Didsync = 0;
	sfsync(f1);
	if(!Didsync)
		terror("Did not see SF_SYNC2\n");

	Didsync = 0;
	sfdisc(f1,NIL(Sfdisc_t*));
	if(Didsync)
		terror("Should not have seen this\n");

	sfseek(f2,(Sfoff_t)0,0);
	sfgetc(f2);
	if((s = sftell(f2)) != 1)
		terror("Wrong sfseek location s=%lld\n",s);
	sfsync(0);
	if(lseek(sffileno(f2),0L,1) != 1)
		terror("Wrong lseek location\n");

	dupf2 = dup(sffileno(f2));
	sfclose(f2);
	if(lseek(dupf2,0L,1) != 1)
		terror("Wrong lseek location2\n");

	rmkpv();
	return 0;
}
