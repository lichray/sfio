#include	"sftest.h"

MAIN()
{
	Sfio_t	*f, *str, *fr, *fw, *sf[2];
	int	c;
	char	*s;
	int	fd[2];

	if(argc > 1)
	{	while((s = sfgetr(sfstdin, '\n', 1)) )
		{	sfputr(sfstdout, s, '\n');
			sfsync(sfstdout);
		}
		exit(0);
	}

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

	if(pipe(fd) < 0)
		terror("Can't create pipe\n");

	if(!(fr = sfnew(fr,NIL(Void_t*),(size_t)SF_UNBOUND,fd[0],SF_READ)) )
		terror("Can't create stream\n");

	if(write(fd[1],"0123456789",10) != 10)
		terror("Can't write to pipe\n");

	if(sfpoll(&fr,1,1000) != 1)
		terror("Data should be available\n");

	s = sfprints("%s 1", argv[0]);
	if(!(f = sfpopen(0, s, "w+")) )
		terror("Can't create read/write process");
	if(sfwrite(f, "abc\n",4) != 4)
		terror("Writing to pipe");

	if(sfpoll(&f, 1, 0) != 1)
		terror("Poll should succeed\n");
	if(sfvalue(f)&SF_READ)
		terror("Read should not be ready\n");
	if(!(sfvalue(f)&SF_WRITE) )
		terror("Write should be ready\n");
	if(sfsync(f) < 0)
		terror("Bad sync");

	if(sfpoll(&f, 1, 1000) != 1)
		terror("Poll should succeed2\n");
	if(!(sfvalue(f)&SF_READ) )
		terror("Read should be ready\n");
	if(!(sfvalue(f)&SF_WRITE) )
		terror("Write should be ready\n");
	if(!(s = sfgetr(f,'\n',1)) || strcmp(s, "abc") != 0)
		terror("Bad read");

	TSTEXIT(0);
}
