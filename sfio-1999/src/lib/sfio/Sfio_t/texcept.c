#include	"sftest.h"

static int	Type;

#if __STD_C
static int except(Sfio_t* f, int type, Void_t* data, Sfdisc_t* disc)
#else
static int except(f, type, data, disc)
Sfio_t*		f;
int		type;
Void_t*		data;
Sfdisc_t*	disc;
#endif
{
	switch(type)
	{
	case SF_WRITE :
		return 0;
	case SF_CLOSE:
		if(Type == SF_CLOSE)
			return 0;
	case SF_SYNC:
		if(Type == SF_CLOSE)
			return 0;
	}

	Type = type;
	return -1;
}

#if __STD_C
static int except2(Sfio_t* f, int type, Void_t* data, Sfdisc_t* disc)
#else
static int except2(f, type, data, disc)
Sfio_t*		f;
int		type;
Void_t*		data;
Sfdisc_t*	disc;
#endif
{	return 0;
}

#if __STD_C
static int except3(Sfio_t* f, int type, Void_t* data, Sfdisc_t* disc)
#else
static int except3(f, type, data, disc)
Sfio_t*		f;
int		type;
Void_t*		data;
Sfdisc_t*	disc;
#endif
{	if(type == SF_LOCKED)
	{	Type = type;
		return -1;
	}
	return 0;
}
#if __STD_C
static ssize_t readfunc(Sfio_t* f, Void_t* buf, size_t n, Sfdisc_t* disc)
#else
static ssize_t readfunc(f, buf, n, disc)
Sfio_t*	f;
Void_t* buf;
size_t	n;
Sfdisc_t* disc;
#endif
{
	if(sfgetc(f) >= 0)
		terror("Can't access stream here!");
	return 0;
}

static Sfdisc_t	Disc, Disc2;

main()
{
	Sfio_t*	f;
	char	buf[1024];
	char	rbuf[4*1024];
	int	i;

	if(!(f = sfopen(NIL(Sfio_t*), Kpv[0], "w")) )
		terror("Can't open file\n");
	sfset(f,SF_IOCHECK,1);

	Disc.exceptf = except;
	if(!sfdisc(f,&Disc) )
		terror("Pushing discipline failed\n");

	sfdisc(f,&Disc);
	if(Type != SF_DPUSH)
		terror("Did not get push event\n");

	sfdisc(f,NIL(Sfdisc_t*));
	if(Type != SF_DPOP)
		terror("Did not get pop event\n");

	sfwrite(f,"123",3);
	sfsync(f);
	if(Type != SF_SYNC)
		terror("Did not get sync event\n");

	sfwrite(f,"123",3);
	sfpurge(f);
	if(Type != SF_PURGE)
		terror("Did not get purge event\n");

	sfclose(f);
	if(Type != SF_CLOSE)
		terror("Did not get close event\n");

	sfclose(f);
	if(Type != SF_FINAL)
		terror("Did not get final event\n");

	if(!(f = sfopen(NIL(Sfio_t*), Kpv[0], "r")) )
		terror("Can't open file\n");
	Disc2.readf = readfunc;
	Disc2.exceptf = except3;
	sfdisc(f,&Disc2);
	if(sfgetc(f) >= 0)
		terror("There should be no data here\n");
	if(Type != SF_LOCKED)
		terror("Did not get lock event\n");

	/* test to see if sfclose() preserves seek location */
	if(!(f = sftmp(0)) )
		terror("Can't create temp file\n");
	sfsetbuf(f,buf,sizeof(buf));
	for(i = 0; i < sizeof(rbuf); ++i)
		rbuf[i] = i;
	sfwrite(f,rbuf,sizeof(rbuf));
	sfset(f,SF_WRITE,0);

	Disc.exceptf = except2;
	sfdisc(f,&Disc);
	sfseek(f,(Sfoff_t)0,0);
	if(sfread(f,rbuf,4) != 4)
		terror("reading 4 bytes\n");
	for(i = 0; i < 4; ++i)
		if(rbuf[i] != i)
			terror("wrong 4 bytes\n");
	sfsync(f);
	if((i = dup(sffileno(f))) < 0)
		terror("Can't dup file descriptor\n");

	sfclose(f);
	if(lseek(i,0,1) != 4)
		terror("Wrong seek location\n");

	rmkpv();

	return 0;
}
