#include	"sfstdio.h"

/*	Set IO position pointer
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fsetpos(reg FILE* f, reg fpos_t* pos)
#else
int fsetpos(f, pos)
reg FILE*	f;
reg fpos_t*	pos;
#endif
{
	reg Sfio_t*	sf;

	if(!pos || *pos < 0 || !(sf = SFSTREAM(f)))
		return -1;

	return (fpos_t)sfseek(sf, (Sfoff_t)(*pos), SEEK_SET|SF_SHARE) == *pos ? 0 : -1;
}
