#include	"sfstdio.h"

/*	Get current stream position.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int fgetpos(reg FILE* f, reg stdfpos_t* pos)
#else
int fgetpos(f, pos)
reg FILE*	f;
reg stdfpos_t*	pos;
#endif
{
	reg Sfio_t*	sf;

	if(!(sf = _sfstream(f)))
		return -1;
	return (*pos = (stdfpos_t)sfseek(sf,(Sfoff_t)0,SEEK_CUR|SF_SHARE)) >= 0 ? 0 : -1;
}
