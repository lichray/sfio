#include	"sfstdio.h"

/*	Tell current IO position pointer
**	Written by Kiem-Phong Vo
*/

#if __STD_C
long ftell(reg FILE* f)
#else
long ftell(f)
reg FILE*	f;
#endif
{
	reg Sfio_t*	sf;

	if(!(sf = SFSTREAM(f)))
		return -1L;

	return (long)sfseek(sf, (Sfoff_t)0, SEEK_CUR|SF_SHARE);
}
