#include	"sfstdio.h"

/*	Get current stream position.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int fgetpos(reg FILE* fp, reg fpos_t* pos)
#else
int fgetpos(fp, pos)
reg FILE*	fp;
reg fpos_t*	pos;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	return (*pos = (fpos_t)sfseek(sp, (Sfoff_t)0, SEEK_CUR|SF_SHARE)) >= 0 ? 0 : -1;
}
