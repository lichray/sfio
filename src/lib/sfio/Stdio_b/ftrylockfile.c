#include	"sfstdio.h"

/*	Read a set of data
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int ftrylockfile(FILE* f)
#else
int ftrylockfile(f)
reg FILE*	f;
#endif
{
	Sfio_t*	sf;

	if(!(sf = SFSTREAM(f)))
		return 0;

	return sfmutex(sf, SFMTX_TRYLOCK);
}
