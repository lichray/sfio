#include	"sfstdio.h"

/*	Tell current IO position pointer
**	Written by Kiem-Phong Vo
*/

#if __STD_C
long ftell(reg FILE *fp)
#else
long ftell(fp)
reg FILE	*fp;
#endif
{
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1L;
	_stdclrerr(fp,sp);
	return sftell(sp);
}
