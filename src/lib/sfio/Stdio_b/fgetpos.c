#include	"sfstdio.h"

/*	Get current stream position.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int fgetpos(reg FILE* fp, reg long* pos)
#else
int fgetpos(fp, pos)
reg FILE*	fp;
reg long*	pos;
#endif
{
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	return (*pos = sftell(sp)) >= 0 ? 0 : -1;
}
