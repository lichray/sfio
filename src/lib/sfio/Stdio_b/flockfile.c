#include	"sfstdio.h"

/*	Read a set of data
**	Written by Kiem-Phong Vo
*/

#if __STD_C
void flockfile(FILE* f)
#else
void flockfile(f)
reg FILE*	f;
#endif
{
	Sfio_t*	sf;

	if(!(sf = SFSTREAM(f)))
		return;

	(void)sfmutex(sf, SFMTX_LOCK);
}
