#include	"sftest.h"

#if __STD_C
main(int argc, char** argv)
#else
main(argc, argv)
int     argc;
char    **argv;
#endif
{
	Sfio_t	*str, *fr, *fw, *sf[2];
	char	*s;
	int	c;
	int	fd[2];

	if(!(str = sfopen(NIL(Sfio_t*),"abc","s")) )
		terror("Opening string stream\n");

	if(pipe(fd) < 0)
		terror("pipe failed\n");

	if(!(fr = sfnew(NIL(Sfio_t*),NIL(Void_t*),(size_t)SF_UNBOUND,
			 fd[0],SF_READ)) )
		terror("Opening read pipe stream\n");
	if(!(fw = sfnew(NIL(Sfio_t*),NIL(Void_t*),(size_t)SF_UNBOUND,
			 fd[1],SF_WRITE)) )
		terror("Opening write pipe stream\n");

	sf[0] = fr;
	sf[1] = str;
	if((c = sfpoll(sf,2,0)) != 1 || sf[0] != str)
		terror("Only str should be available c=%d\n",c);

	sf[0] = fr;
	if(sfpoll(sf,1,0) != 0 )
		terror("Pipe stream should not be ready\n");

	sfputc(fw,'a'); sfsync(fw);
	sf[0] = fr;
	if(sfpoll(sf,1,0) != 1 )
		terror("Pipe read should be ready\n");
	if((c = sfgetc(fr)) != 'a')
		terror("Didn't get back right data\n");

	sf[0] = fr;
	sf[1] = str;
	if(sfpoll(sf,2,0) != 1 || sf[0] != str)
		terror("Only str should be available2\n");

	sf[0] = fw;
	sf[1] = str;
	if(sfpoll(sf,2,0) != 2)
		terror("Both str&pipe write should be available\n");

	sfgetc(str); sfgetc(str); sfgetc(str);
	sfstack(fr,str);

	if(sfpoll(&fr,1,0) != 0)
		terror("Nothing should be available\n");

	if(pipe(fd) < 0)
		terror("Can't create pipe\n");

	if(!(fr = sfnew(fr,NIL(Void_t*),SF_UNBOUND,fd[0],SF_READ)) )
		terror("Can't create stream\n");

	if(write(fd[1],"0123456789",10) != 10)
		terror("Can't write to pipe\n");

	if(sfpoll(&fr,1,1000) != 1)
		terror("Data should be available\n");

	return 0;
}
