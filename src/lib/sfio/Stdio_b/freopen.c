#include	"sfstdio.h"

/*	Reopening a stream
**	Written by Kiem-Phong Vo
*/


#if __STD_C
FILE* freopen(char*name, const char* mode, reg FILE* f)
#else
FILE* freopen(name,mode,f)
reg char*	name;
reg char*	mode;
reg FILE*	f;
#endif
{
	Sfio_t*	sf;

	if(f && (sf = _sfstream(f)) )
		_sfunmap(f);

	if(!(sf = sfopen(sf, name, mode)) )
		return NIL(FILE*);
	else
	{	int	uflag;
		_sftype(mode, NIL(int*), &uflag);	
		if(!uflag) 
			sf->flags |= SF_MTSAFE;

		if(_stdstream(sf,f) != f)
		{	sfclose(sf);
			return NIL(FILE*);
		}
		else	return f;
	}
}
