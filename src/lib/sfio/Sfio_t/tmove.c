#include	"sftest.h"

_BEGIN_EXTERNS_
extern Void_t*	sbrk _ARG_((int));
_END_EXTERNS_

#if __STD_C
main(void)
#else
main()
#endif
{
	char	*s = "1234567890\n";
	int	n, i;
	Sfio_t	*f;
	char	buf[1024];
	char*	addr;

	if(sfopen(sfstdout,"xxx","w+") != sfstdout)
		terror("Opening output file xxx\n");
	for(i = 0; i < 10000; ++i)
		if(sfputr(sfstdout,s,-1) < 0)
			terror("Writing data\n");

	if(!(f = sfopen((Sfio_t*)0,"yyy","w")))
		terror("Opening output file yyy\n");

	sfseek(sfstdout,0L,0);
	if((n = sfmove(sfstdout,f,-1,'\n')) != i)
		terror("Move %d lines, Expect %d\n",n,i);

	sfseek(sfstdout,0L,0);
	sfseek(f,0L,0);
	sfsetbuf(sfstdout,buf,sizeof(buf));
	if((n = sfmove(sfstdout,f,-1,'\n')) != i)
		terror("Move %d lines, Expect %d\n",n,i);

	sfopen(sfstdin,"xxx","r");
	sfopen(sfstdout,"yyy","w");
	sfmove(sfstdin,sfstdout,-1,-1);
	if(!sfeof(sfstdin))
		terror("Sfstdin is not eof\n");
	if(sferror(sfstdin))
		terror("Sfstdin is in error\n");
	if(sferror(sfstdout))
		terror("Sfstdout is in error\n");

	sfseek(sfstdin,0L,0);
	sfseek(sfstdout,0L,0);
	sfsetbuf(sfstdin,buf,sizeof(buf));

	addr = (char*)sbrk(0);
	sfmove(sfstdin,sfstdout,(long)((unsigned long)(~0L)>>1),-1);
	if(((char*)sbrk(0)-addr) > 16*(_Sfpage > 0 ? _Sfpage : SF_PAGE) )
		terror("Too much space allocated in sfmove\n");

	if(!sfeof(sfstdin))
		terror("Sfstdin is not eof2\n");
	if(sferror(sfstdin))
		terror("Sfstdin is in error2\n");
	if(sferror(sfstdout))
		terror("Sfstdout is in error2\n");

	system("rm xxx yyy");

	return 0;
}
