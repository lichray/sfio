#include	"sftest.h"

#undef fork	/* make sure fork() is used, not vfork() */

static int	Count = 0;
static Sfdisc_t	Disc;

#if __STD_C
void count(Sfio_t* f, int type, int fd)
#else
void count(f, type, fd)
Sfio_t* f;
int type;
int fd;
#endif
{
	if(fd >= 0)
		Count += 1;
}

main()
{
	Sfio_t	*f;
	char	*s;

	/* let two run concurrently */
	fork();

	f = sftmp((size_t)SF_UNBOUND);

	sfputr(f,"1234",'\n');	/* write a string into it */
	sfseek(f,(Sfoff_t)0,0);		/* get back so we can read the string */
	s = sfreserve(f,-1,0);
	if(sfvalue(f) != 5)
		terror("Get n=%d, expect n=5\n", sfvalue(f));

	sfseek(f,(Sfoff_t)10,1);	/* seek to extend buffer */
	if(s = sfreserve(f,-1,0))
		terror("Get n=%d, expect n=0\n", sfvalue(f));

	sfset(f,SF_READ,0);	/* turn off read mode so stream is write only */

	sfseek(f,(Sfoff_t)(-10),1);	/* back 10 places to get space to write */
	if(!(s = sfreserve(f,-1,1)) || sfwrite(f,s,0) != 0)
		terror("Get n=%d, expect n > 0\n", sfvalue(f));
	strcpy(s,"5678\n");

	sfset(f,SF_READ,1);
	sfseek(f,(Sfoff_t)0,0);		/* read 1234\n5678\n */
	if(!(s = sfreserve(f,-1,1)) || sfread(f,s,0) != 0)
		terror("Get n=%d, expect n > 0\n", sfvalue(f));
	if(strncmp(s,"1234\n5678\n",10) != 0)
		terror("Get wrong string\n");
	sfclose(f);

	sfnotify(count);
	if(!(f = sftmp(0)) )
		terror("sftmp failed 1\n");
	if(Count != 1)
		terror("wrong count 1, count=%d\n", Count);
	sfclose(f);
	if(Count != 2)
		terror("wrong count 2 count=%d\n", Count);

	if(!(f = sftmp(8)) )
		terror("sftmp failed 2\n");
	if(Count != 2)
		terror("wrong count 2.2 count=%d\n", Count);
	sfdisc(f,&Disc);
	if(Count != 3)
		terror("wrong count 3 count=%d\n", Count);
	sfclose(f);
	if(Count != 4)
		terror("wrong count 4 count=%d\n", Count);

	if(!(f = sftmp(8)) )
		terror("sftmp failed 3\n");
	if(Count != 4)
		terror("wrong count 4.2 count=%d\n", Count);
	sfwrite(f,"0123456789",10);
	if(Count != 5)
		terror("wrong count 5 count=%d\n", Count);
	sfclose(f);
	if(Count != 6)
		terror("wrong count 6 count=%d\n", Count);

	if(!(f = sftmp(1024)) )
		terror("sftmp failed 4\n");
	sfwrite(f,"1234567890",10);
	sfseek(f,(Sfoff_t)0,0);
	if(sfsize(f) != 10)
		terror("Wrong size1\n");
	sfdisc(f,SF_POPDISC);
	if(sfsize(f) != 10)
		terror("Wrong size2\n");
	s = sfreserve(f,-1,0);
	if(sfvalue(f) != 10 || strncmp(s,"1234567890",10) != 0)
		terror("did not create correct real file\n");

	return 0;
}
