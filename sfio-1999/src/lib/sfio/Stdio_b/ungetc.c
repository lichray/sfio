#include	"sfstdio.h"

/*	Put back an input byte.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int ungetc(int c, FILE* fp)
#else
int ungetc(c,fp)
reg int		c;
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	return sfungetc(sp,c);
}
