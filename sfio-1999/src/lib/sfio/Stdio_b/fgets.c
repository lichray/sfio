#include	"sfstdio.h"

/*	Get a line.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
char* fgets(char* buf, int n, FILE* fp)
#else
char* fgets(buf,n,fp)
char*	buf;
int	n;
FILE*	fp;
#endif
{
	reg Sfio_t*	sp;
	reg char*	rv;

	if(!(sp = _sfstream(fp)))
		return NIL(char*);

	_stdclrerr(fp,sp);
	if(!(rv = _stdgets(sp,buf,n,0)))
	{	if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
	}

	return rv;
}
