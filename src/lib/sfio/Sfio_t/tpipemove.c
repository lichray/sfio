#include	"sftest.h"

main()
{
	int	fd[2];
	Sfio_t	*fr, *fw;
	char	*s;
	int	n, w;

	if(pipe(fd) < 0)
		terror("Can't open pipe\n");

	if(!(fr = sfnew(NIL(Sfio_t*),NIL(char*),-1,fd[0],SF_READ)) ||
	   !(fw = sfnew(NIL(Sfio_t*),NIL(char*),-1,fd[1],SF_WRITE)) )
		terror("Can't open pipe streams\n");
	sfset(fr,SF_SHARE,1);

	if(sfopen(sfstdout,"xxx","w") != sfstdout)
		terror("Can't open xxx for write\n");
	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Can't open xxx for read\n");

	for(n = 0; n < 100; ++n)
		if((w = sfwrite(fw,"123456789\n",10)) != 10)
			terror("Writing to pipe w=%d\n",w);

	if((n = (int)sfmove(fr,sfstdout,100L,'\n')) != 100)
		terror("sfmove failed n=%d\n", n);
	sfclose(sfstdout);

	for(n = 0; n < 100; ++n)
	{	if(!(s = sfgetr(sfstdin,'\n',1)) )
			terror("Can`t read data from xxx\n");
		if(strcmp(s,"123456789") != 0)
			terror("Wrong data\n");
	}

	unlink("xxx");
	exit(0);
}
