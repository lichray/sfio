#include	"sftest.h"

#if __STD_C
main(void)
#else
main()
#endif
{
	Sfio_t	*f;
	char	*s, *endos, *os = "one\ntwo\nthree\n";
	int	n;

	if(!(f = sfpopen((Sfio_t*)0,"cat > /tmp/kpv.x","w")))
		terror("Opening for write\n");
	if(sfwrite(f,os,strlen(os)) != strlen(os))
		terror("Writing\n");
	sfclose(f);
	if(!(f = sfpopen((Sfio_t*)0,"cat < /tmp/kpv.x","r")))
		terror("Opening for read\n");
	sleep(1);

	endos = os + strlen(os);
	while(s = sfgetr(f,'\n',0))
	{	n = sfslen();
		if(strncmp(s,os,n) != 0)
		{	s[n-1] = os[n-1] = 0;
			terror("Input=%s, Expect=%s\n",s,os);
		}
		os += n;
	}

	if(os != endos)
		terror("Does not match all data, left=%s\n",os);

	system("rm /tmp/kpv.x");

	return 0;
}
