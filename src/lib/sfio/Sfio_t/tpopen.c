#include	"sftest.h"
#include	<signal.h>

main()
{
	Sfio_t	*f;
	char	*s, *endos, *os = "one\ntwo\nthree\n";
	int	n;

	if(!(f = sfpopen((Sfio_t*)0, sfprints("cat > %s",Kpv[0]), "w")))
		terror("Opening for write\n");
	if(sfwrite(f,os,strlen(os)) != (ssize_t)strlen(os))
		terror("Writing\n");

#ifdef SIGPIPE
	{	void(* handler)_ARG_((int));
		if((handler = signal(SIGPIPE,SIG_DFL)) == SIG_DFL)
			terror("Wrong signal handler\n");
		signal(SIGPIPE,handler);
	}
#endif

	sfclose(f);

#ifdef SIGPIPE
	{	void(* handler)_ARG_((int));
		if((handler = signal(SIGPIPE,SIG_DFL)) != SIG_DFL)
			terror("Wrong signal handler2\n");
		signal(SIGPIPE,handler);
	}
#endif

	if(!(f = sfpopen((Sfio_t*)0, sfprints("cat < %s",Kpv[0]), "r")))
		terror("Opening for read\n");
	sleep(1);

	endos = os + strlen(os);
	while(s = sfgetr(f,'\n',0))
	{	n = sfvalue(f);
		if(strncmp(s,os,n) != 0)
		{	s[n-1] = os[n-1] = 0;
			terror("Input=%s, Expect=%s\n",s,os);
		}
		os += n;
	}

	if(os != endos)
		terror("Does not match all data, left=%s\n",os);

	rmkpv();
	return 0;
}
