#include	"sftest.h"


#if __STD_C
upper(Sfio_t* f, Void_t* buf, int n, Sfdisc_t* disc)
#else
upper(f,buf,n,disc)
Sfio_t		*f;
Void_t*		buf;
int		n;
Sfdisc_t	*disc;
#endif
{
	int	i;
	char*	s = (char*)buf;

	if(sfgetc(f) >= 0)
		terror("Stream should be locked\n");

	if((n = sfrd(f,buf,n,disc)) <= 0)
		return n;
	for(i = 0; i < n; ++i)
		if(s[i] >= 'a' && s[i] <= 'z')
			s[i] = (s[i]-'a') + 'A';
	return n;
}

#if __STD_C
wupper(Sfio_t* f, const Void_t* buf, int n, Sfdisc_t* disc)
#else
wupper(f,buf,n,disc)
Sfio_t		*f;
Void_t*		buf;
int		n;
Sfdisc_t	*disc;
#endif
{
	int	i;
	char*	s = (char*)buf;

	if(sfputc(f,0) >= 0)
		terror("Stream should be locked2\n");

	for(i = 0; i < n; ++i)
		if(s[i] >= 'a' && s[i] <= 'z')
			s[i] = (s[i]-'a') + 'A';
	return sfwr(f,buf,n,disc);
}

#if __STD_C
lower(Sfio_t* f, Void_t* buf, int n, Sfdisc_t* disc)
#else
lower(f,buf,n,disc)
Sfio_t		*f;
Void_t*		buf;
int		n;
Sfdisc_t	*disc;
#endif
{
	int	i;
	char*	s = (char*)buf;

	if(sfgetc(f) >= 0)
		terror("Stream should be locked\n");

	if((n = sfrd(f,buf,n,disc)) <= 0)
		return n;
	for(i = 0; i < n; ++i)
		if(s[i] >= 'A' && s[i] <= 'Z')
			s[i] = (s[i]-'A') + 'a';
	return n;
}

Sfdisc_t	Wdisc = {(Sfread_f)0, wupper, (Sfseek_f)0, (Sfexcept_f)0, (Sfdisc_t*)0};
Sfdisc_t	Udisc = {upper, wupper, (Sfseek_f)0, (Sfexcept_f)0, (Sfdisc_t*)0};
Sfdisc_t	Ldisc = {lower, (Sfwrite_f)0, (Sfseek_f)0, (Sfexcept_f)0, (Sfdisc_t*)0};

#if __STD_C
main(void)
#else
main()
#endif
{
	Sfio_t	*f;
	char	*l, *u, *s;
	int	n, r;
	int	flags;
	char	buf[1024], low[64], up[64];

	sfsetbuf(sfstdin,buf,sizeof(buf));
	flags = sfset(sfstdin,0,0);
	sfdisc(sfstdin,&Ldisc);
	if(sfset(sfstdin,0,0) != flags)
		terror("Flags changed after discipline setting\n");

	l = low;
	strcpy(l, "abcdefghijklmnopqrstuvwxyz");
	u = up;
	strcpy(u, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	n = strlen(l);

	if(!(f = sfopen(NIL(Sfio_t*),"xxx","w+")))
		terror("Creating temp file\n");
	if((r = sfwrite(f,l,n)) != n)
		terror("Writing data %d\n",r);
	sfsync(f);

	sfseek(f,0L,0);
	sfdisc(f,&Udisc);
	if(!(s = sfreserve(f,n,0)) )
		terror("Reading string1\n");
	if(strncmp(s,u,n) != 0)
	{	s[n-1] = 0;
		terror("Input1=%s, Expect=%s\n",s,u);
	}

	sfseek(f,0L,0);
	sfdisc(f,&Ldisc);
	if(!(s = sfreserve(f,n,0)) )
		terror("Reading string2\n");
	if(strncmp(s,l,n) != 0)
	{	s[n-1] = 0;
		terror("Input2=%s, Expect=%s\n",s,l);
	}

	if(!(f = sfopen(NIL(Sfio_t*),"xxx","w+")) )
		terror("Opening file\n");
	sfdisc(f,&Wdisc);
	if(sfputr(f,low,'\n') < 0)
		terror("Writing data\n");
	if(sfseek(f,0L,0) != 0L)
		terror("Seeking\n");
	if(!(s = sfgetr(f,'\n',1)))
		terror("sfgetr\n");
	if(strcmp(s,up) != 0)
		terror("Bad data\n");
	system("rm xxx >/dev/null 2>&1");

	return 0;
}
