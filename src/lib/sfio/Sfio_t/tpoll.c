#include	"sftest.h"

#if __STD_C
main(int argc, char** argv)
#else
main(argc, argv)
int     argc;
char    **argv;
#endif
{
	Sfio_t	*str, *cat, *sf[2];
	char	*s;
	int	fd[2];

	if(argc > 1)
	{	sfset(sfstdout,SF_SHARE,1);
                while((s = sfreserve(sfstdin,-1,0)) )
                       sfwrite(sfstdout,s,sfslen());
		exit(0);
	}

	if(!(str = sfopen(NIL(Sfio_t*),"abc","s")) )
		terror("Opening string stream\n");
	if(!(cat = sfpopen(NIL(Sfio_t*),sfprints("%s -p",argv[0]),"r+")) )
		terror("Opening coprocess\n");

	sfset(cat,SF_READ,1);
	sf[0] = cat;
	sf[1] = str;
	if(sfpoll(sf,2,0) != 1 || sf[0] != str)
		terror("Only str should be available\n");

	sf[0] = cat;
	if(sfpoll(sf,1,0) != 0 )
		terror("Coprocess should not be ready\n");

	sfputc(cat,'a');
	sfset(cat,SF_READ,1);
	sleep(1);
	sf[0] = cat;
	if(sfpoll(sf,1,0) != 1 )
		terror("Coprocess should be ready\n");
	if(sfgetc(cat) != 'a')
		terror("Didn't get back right data\n");

	sf[0] = cat;
	sf[1] = str;
	if(sfpoll(sf,2,0) != 1 || sf[0] != str)
		terror("Only str should be available2\n");

	sfset(cat,SF_WRITE,1);
	sf[0] = cat;
	sf[1] = str;
	if(sfpoll(sf,2,0) != 2)
		terror("Both str&coprocess should be available\n");

	sfset(cat,SF_READ,1);
	sfgetc(str); sfgetc(str); sfgetc(str);
	sfstack(cat,str);

	if(sfpoll(&cat,1,0) != 0)
		terror("Nothing should be available\n");

	if(pipe(fd) < 0)
		terror("Can't create pipe\n");

	if(!(cat = sfnew(cat,NIL(char*),-1,fd[0],SF_READ)) )
		terror("Can't create stream\n");

	if(write(fd[1],"0123456789",10) != 10)
		terror("Can't write to pipe\n");

	if(sfpoll(&cat,1,1000) != 1)
		terror("Data should be available\n");

	exit(0);
}
