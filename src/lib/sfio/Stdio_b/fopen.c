#include	"sfstdio.h"

/*	Opening a stream
**	Written by Kiem-Phong Vo
*/

#if __STD_C
FILE* fopen(char* name,const char* mode)
#else
FILE* fopen(name,mode)
char*	name;
char*	mode;
#endif
{
	reg FILE*	f;
	reg Sfio_t*	sf;

	if(!(sf = sfopen(NIL(Sfio_t*), name, mode)) )
		f = NIL(FILE*);
	else if(!(f = _stdstream(sf)))
		sfclose(sf);
	else
	{	int	uflag;
		_sftype(mode, NIL(int*), &uflag);
		if(!uflag)
			sf->flags |= SF_MTSAFE;
	}

	return(f);
}
