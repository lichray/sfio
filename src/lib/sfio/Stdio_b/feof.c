#include	"sfstdio.h"

/*	Return the eof condition if any.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int feof(FILE* fp)
#else
int feof(fp)
FILE*	fp;
#endif
{
	reg Sfio_t*	sp;
	
	if(!(sp = _sfstream(fp)))
		return -1;
	return sfeof(sp);
}
