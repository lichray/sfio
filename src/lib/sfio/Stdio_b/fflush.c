#include	"sfstdio.h"

/*	Flushing buffered output data.
**	This has been modified to work with both input&output streams.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int fflush(reg FILE* f)
#else
int fflush(f)
reg FILE*	f;
#endif
{
	reg Sfio_t*	sf;

	if(!f)
		return sfsync(NIL(Sfio_t*));
	if(!(sf = SFSTREAM(f)))
		return -1;

	(void)sfseek(sf, (Sfoff_t)0, SEEK_CUR|SF_PUBLIC);

	return (sfsync(sf) < 0 || sfpurge(sf) < 0) ? -1 : 0;
}

#if _lib_fflush_unlocked && !_done_fflush_unlocked && !defined(fflush)
#define _done_fflush_unlocked	1
#define fflush	fflush_unlocked
#include	"fflush.c"
#undef fflush
#endif
