#include	"sfstdio.h"

/*	Junk all buffered data
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int fpurge(reg FILE* fp)
#else
int fpurge(fp)
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);

	return sfpurge(sp);
}
