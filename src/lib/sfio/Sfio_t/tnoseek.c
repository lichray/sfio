#include	"sftest.h"

char*	buffer;
int	size;
int	count;

#if __STD_C
long discseek(Sfio_t* f, long loc, int offset, Sfdisc_t* disc)
#else
long discseek(f,loc,offset,disc)
Sfio_t*		f;
long		loc;
int		offset;
Sfdisc_t*	disc;
#endif
{
	return 0L;
}

#if __STD_C
int discwrite(Sfio_t* f, const Void_t* s, int n, Sfdisc_t* disc)
#else
int discwrite(f,s,n,disc)
Sfio_t*		f;
Void_t*		s;
int		n;
Sfdisc_t*	disc;
#endif
{
	buffer = (char*)s;
	size = n;
	count += 1;
	return n;
}

Sfdisc_t seekable = { (Sfread_f)0, discwrite, discseek, (Sfexcept_f)0 };

main()
{
	char	buf[1024];

	sfsetbuf(sfstdout,buf,sizeof(buf));
	sfset(sfstdout,SF_LINE,0);

	if(sfdisc(sfstdout,&seekable) != &seekable)
		terror("Can't set discipline\n");
	if(sfseek(sfstdout,0,0) < 0)
		terror("Sfstdout should be seekable\n");
	if(sfwrite(sfstdout,"123\n",4) != 4)
		terror("Can't write\n");
	if(sfwrite(sfstdout,"123\n",4) != 4)
		terror("Can't write\n");
	if(sfdisc(sfstdout,NIL(Sfdisc_t*)) != &seekable)
		terror("Can't pop discipline\n");

	if(buffer != buf || size != 8 || count != 1)
		terror("Wrong calls to write\n");

	exit(0);
}
