#include	"sfstdio.h"

/*	Set line mode
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int setlinebuf(reg FILE* fp)
#else
int setlinebuf(fp)
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	sfset(sp,SF_LINE,1);
	return(0);
}
