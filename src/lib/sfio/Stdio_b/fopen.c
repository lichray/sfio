#include	"sfstdio.h"

/*	Opening a stream
**	Written by Kiem-Phong Vo
*/

#if __STD_C
FILE* fopen(char *name,const char *mode)
#else
FILE* fopen(name,mode)
char	*name;
char	*mode;
#endif
{
	reg FILE	*fp;
	reg Sfio_t	*sp;

	sp = sfopen((Sfio_t*)0, name, mode);
	if(!(fp = _stdstream(sp)))
	{	sfclose(sp);
		return NIL(FILE*);
	}

	return(fp);
}
