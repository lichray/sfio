#include	"sfstdio.h"

/*	Flushing buffered output data.
**	This has been modified to work with both input&output streams.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int fflush(reg FILE *fp)
#else
int fflush(fp)
reg FILE *fp;
#endif
{
	reg Sfio_t	*sp;

	if(!fp)
		return sfsync(NIL(Sfio_t*));
	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	return sfsync(sp);
}
