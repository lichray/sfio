#include	"sfstdio.h"

/*	Clear error status from a stream
**
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int clearerr(FILE* fp)
#else
int clearerr(fp)
FILE*	fp;
#endif
{
	reg Sfio_t*	sp;
	
	if(!(sp = _sfstream(fp)))
		return -1;
	return _stdclrerr(fp,sp);
}
