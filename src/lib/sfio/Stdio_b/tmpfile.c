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
	reg Sfio_t*	sf;
	reg FILE*	f;

	if(!(sf = sftmp(0)))
		f = NIL(FILE*);
	else if(!(f = _stdstream(sf, NIL(FILE*))))
		sfclose(sf);
	else	sf->flags |= SF_MTSAFE;

	return f;
}
