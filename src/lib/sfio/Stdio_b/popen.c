#include	"sfstdio.h"

/*	Opening a process as a stream
**	Written by Kiem-Phong Vo
*/


#if __STD_C
FILE* popen(const char* string,const char* mode)
#else
FILE* popen(string,mode)
char*	string;
char*	mode;
#endif
{
	reg Sfio_t*	sp;
	reg FILE*	fp;

	if(!(sp = sfpopen((Sfio_t*)(-1), string, mode)))
		return NIL(FILE*);
	if(!(fp = _stdstream(sp)))
	{	sfclose(sp);
		return NIL(FILE*);
	}

	return(fp);
}
