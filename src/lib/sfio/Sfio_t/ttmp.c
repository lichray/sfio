#include	"sftest.h"

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
	Count += 1;
}

#if __STD_C
main(void)
#else
main()
#endif
{
	Sfio_t	*f;
	char	*s;

	f = sftmp(SF_UNBOUND);

	sfputr(f,"1234",'\n');	/* write a string into it */
	sfseek(f,0L,0);		/* get back so we can read the string */
	s = sfreserve(f,-1,0);
	if(sfslen() != 5)
		terror("Get n=%d, expect n=5\n", sfslen());

	sfseek(f,10L,1);	/* seek to extend buffer */
	if(s = sfreserve(f,-1,0))
		terror("Get n=%d, expect n=0\n", sfslen());

	sfset(f,SF_READ,0);	/* turn off read mode so stream is write only */

	sfseek(f,-10L,1);	/* back 10 places to get space to write */
	if(!(s = sfreserve(f,-1,1)) || sfwrite(f,s,0) != 0)
		terror("Get n=%d, expect n > 0\n", sfslen());
	strcpy(s,"5678\n");

	sfset(f,SF_READ,1);
	sfseek(f,0L,0);		/* read 1234\n5678\n */
	if(!(s = sfreserve(f,-1,1)) || sfread(f,s,0) != 0)
		terror("Get n=%d, expect n > 0\n", sfslen());
	if(strncmp(s,"1234\n5678\n",10) != 0)
		terror("Get wrong string\n");
	sfclose(f);

	sfnotify(count);
	f = sftmp(0);
	if(Count != 1)
		terror("notify function should have been called 1 time\n");
	sfclose(f);
	if(Count != 2)
		terror("notify function should have been called 2 times\n");

	f = sftmp(8);
	if(Count != 3)
		terror("notify function should have been called 3 times\n");
	sfdisc(f,&Disc);
	if(Count != 4)
		terror("notify function should have been called 4 times\n");
	sfclose(f);
	if(Count != 5)
		terror("notify function should have been called 5 times\n");

	f = sftmp(8);
	if(Count != 6)
		terror("notify function should have been called 6 times\n");
	sfwrite(f,"0123456789",10);
	if(Count != 7)
		terror("notify function should have been called 7 times\n");
	sfclose(f);
	if(Count != 8)
		terror("notify function should have been called 8 times\n");

	f = sftmp(1024);
	sfwrite(f,"1234567890",10);
	sfseek(f,0L,0);
	if(sfsize(f) != 10)
		terror("Wrong size1\n");
	sfdisc(f,SF_POPDISC);
	if(sfsize(f) != 10)
		terror("Wrong size2\n");
	s = sfreserve(f,-1,0);
	if(sfslen() != 10 || strncmp(s,"1234567890",10) != 0)
		terror("did not create correct real file\n");

	return 0;
}
