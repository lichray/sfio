#include	"sfstdio.h"

/*	Flushing buffered output data.
**	This has been modified to work with both input&output streams.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int fflush(reg FILE* fp)
#else
int fflush(fp)
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;

	if(!fp)
		return sfsync(NIL(Sfio_t*));
	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);

#if _xopen_stdio
	(void)sfseek(sp, (Sfoff_t)0, SEEK_CUR|SF_PUBLIC);
#else
	(void)sfseek(sp, (Sfoff_t)0, SEEK_CUR);
#endif

	if(sfsync(sp) < 0 || sfpurge(sp) < 0)
		return -1;
	else	return 0;
}
