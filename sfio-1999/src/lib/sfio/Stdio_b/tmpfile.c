#include	"sfstdio.h"

/*	Creating a temporary stream.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
FILE* tmpfile(void)
#else
FILE* tmpfile()
#endif
{
	reg Sfio_t*	sp;
	reg FILE*	fp;

	if(!(sp = sftmp(0)))
		return NIL(FILE*);
	if(!(fp = _stdstream(sp)))
	{	sfclose(sp);
		return NIL(FILE*);
	}

	return fp;
}
