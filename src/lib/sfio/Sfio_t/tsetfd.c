#include	"sftest.h"

MAIN()
{
	Sfio_t	*f;
	int	fd;

	if(!(f = sfopen((Sfio_t*)0,tstfile(0),"w+")))
		terror("Opening file\n");
	fd = sffileno(f);

	if(sfsetfd(f,-1) != -1 || sffileno(f) != -1)
		terror("setfd1\n");
	if(sfputc(f,'a') >= 0)
		terror("sfputc\n");

	if(sfsetfd(f,fd) != fd)
		terror("setfd2\n");

	if(sfwrite(f,"123456789\n",10) != 10)
		terror("sfwrite\n");

	sfseek(f,(Sfoff_t)0,0);
	if(sfgetc(f) != '1')
		terror("sfgetc1\n");

	if(sfsetfd(f,-1) != -1 || sffileno(f) != -1)
		terror("setfd2\n");
	if(lseek(fd,0L,1) != 1L)
		terror("Bad seek address %d\n",lseek(fd,0L,1));
	if(sfgetc(f) >= 0)
		terror("sfgetc2\n");

	if(sfsetfd(f,fd) != fd)
		terror("setfd2\n");
	if(sfgetc(f) != '2')
		terror("sfgetc3\n");

	TSTRETURN(0);
}
