#include	"sfstdio.h"

/*	Return file number.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int fileno(FILE* fp)
#else
int fileno(fp)
FILE	*fp;
#endif
{
	reg Sfio_t	*sp;
	
	if(!(sp = _sfstream(fp)))
		return -1;
	return sffileno(sp);
}
