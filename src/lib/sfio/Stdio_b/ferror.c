#include	"sfstdio.h"

/*	Return the error condition if any.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int ferror(FILE* fp)
#else
int ferror(fp)
FILE	*fp;
#endif
{
	reg Sfio_t	*sp;
	
	if(!(sp = _sfstream(fp)))
		return -1;
	return sferror(sp);
}
