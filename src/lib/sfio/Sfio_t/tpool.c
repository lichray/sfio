#include	"sftest.h"

static char	Serial[128], *S = Serial;
#if __STD_C
ssize_t writef(Sfio_t* f, const Void_t* buf, size_t n, Sfdisc_t* disc)
#else
ssize_t writef(f, buf, n, disc)
Sfio_t*		f;
Void_t*		buf;
size_t		n;
Sfdisc_t*	disc;
#endif
{
	memcpy((Void_t*)S,buf,n);
	S += n;
	return n;
}
Sfdisc_t	Serialdc = {NIL(Sfread_f), writef, NIL(Sfseek_f), NIL(Sfexcept_f) };

main()
{
	int	i, n, on;
	char	*s, *os;
	char	poolbuf[1024];
	Sfio_t	*f1, *f2, *f3, *f4;

	if(!(f1 = sfopen((Sfio_t*)0,"xxx","w+")) ||
	   !(f2 = sfopen((Sfio_t*)0,"yyy","w"))  ||
	   !(f3 = sfopen((Sfio_t*)0,"zzz","w")))
		terror("Opening files\n");

	if(!(f4 = sfopen((Sfio_t*)0,"xxx","r+")) )
		terror("Opening xxx\n");
	sfungetc(f1,'a');
	sfungetc(f4,'b');
	sfpool(f1,f4,0);
	sfungetc(f1,'a');
	sfpool(f1,NIL(Sfio_t*),0);

	sfsetbuf(f2,poolbuf,sizeof(poolbuf));
	sfsetbuf(f3,poolbuf,sizeof(poolbuf));
	if(!sfpool(f2,f3,0) )
		terror("Setting pool\n");

	os = "1234567890\n";
	on = strlen(os);
	for(i = 0; i < 100; ++i)
		if(sfputr(f1,os,-1) < 0)
			terror("Writing data\n");
	sfseek(f1,(Sfoff_t)0,0);
	for(i = 0; i < 100; ++i)
	{	if(!(s = sfgetr(f1,'\n',1)) || (n = sfvalue(f1)) != on)
			terror("Reading data\n");
		if(sfwrite(f2,s,n) != n)
			terror("Writing to yyy\n");
		if(sfwrite(f3,s,n) != n)
			terror("Writing to zzz\n");
	}
	if(sfclose(f1) < 0 || sfclose(f2) < 0 || sfclose(f3) < 0)
		terror("Closing files\n");

	sfdisc(sfstdout,&Serialdc);
	sfdisc(sfstderr,&Serialdc);
	sfset(sfstdout,SF_LINE,0);
	sfset(sfstderr,SF_LINE,0);
	if(!sfpool(sfstdout,sfstderr,0) )
		terror("sfpool1\n");
	sfputc(sfstdout,'1');
	sfputc(sfstderr,'2');
	sfputc(sfstdout,'3');
	sfputc(sfstderr,'4');
	sfsync(sfstderr);
	if(strcmp(Serial,"1234") != 0)
		terror("Pool not serializing output\n");
	sfdisc(sfstdout,NIL(Sfdisc_t*));
	sfdisc(sfstderr,NIL(Sfdisc_t*));

	sfclose(sfstdout);
	if(!(f1 = sfopen((Sfio_t*)0,"xxx","r")))
		terror("sfopen\n");
	if(!sfpool(f1,sfstderr,0) )
		terror("sfpool2\n");

	system("cmp xxx yyy 2>&1; cmp yyy zzz 2>&1; rm xxx yyy zzz");

	if(!(f1 = sfopen(NIL(Sfio_t*),NIL(char*),"s+")) ||
	   !(f2 = sfopen(NIL(Sfio_t*),NIL(char*),"s+")) ||
	   !(f3 = sfopen(NIL(Sfio_t*),NIL(char*),"s+")) )
		terror("sfopen3\n");
	if(!sfpool(f1,f2,SF_SHARE) || !sfpool(f3,f2,SF_SHARE) )
		terror("sfpool3\n");
	if(sfputc(f3,'z') < 0)
		terror("sfputc to f3\n");
	if(!sfpool(f3,NIL(Sfio_t*),0) )
		terror("sfpool to delete f3\n");
	if(sfputc(f1,'x') < 0)
		terror("sfputc from f1\n");
	if(!sfpool(f1,NIL(Sfio_t*),0) )
		terror("sfpool to delete f1\n");

	return 0;
}
