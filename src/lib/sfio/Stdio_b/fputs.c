#include	"sfstdio.h"

/*	Output a string.
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int fputs(const char* s, FILE* fp)
#else
int fputs(s, fp)
reg char*	s;
reg FILE*	fp;
#endif
{
	reg int		rv;
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfputr(sp,s,-1)) < 0)
		_stderr(fp);
	return rv;
}
