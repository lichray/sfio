#include	"sfstdio.h"

/*	Set IO position pointer
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fsetpos(reg FILE* fp, reg fpos_t* pos)
#else
int fsetpos(fp, pos)
reg FILE*	fp;
reg fpos_t*	pos;
#endif
{
	reg Sfio_t*	sp;

	if(!pos || *pos < 0 || !(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	return (fpos_t)sfseek(sp,(Sfoff_t)(*pos),0) == *pos ? 0 : -1;
}
