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

	if(!(sf = SFSTREAM(f)))
		return NIL(FILE*);
	else if(!(sf = sfopen(sf, name, mode)) )
		return NIL(FILE*);
	else
	{	int	uflag;
		_sftype(mode, NIL(int*), &uflag);	
		if(!uflag) 
			sf->flags |= SF_MTSAFE;
#if _FILE_cnt
		f->std_cnt = 0;
#endif
#if _FILE_r
		f->std_r = 0;
#endif
#if _FILE_w
		f->std_w = 0;
#endif
#if _FILE_readptr
		f->std_readptr = f->std_readend = NIL(uchar*);
#endif
#if _FILE_writeptr
		f->std_writeptr = f->std_writeend = NIL(uchar*);
#endif
#if _FILE_flag
		f->std_flag = 0;
#endif
#if _FILE_file
		f->std_file = sffileno(sf);
#endif
	}

	return f;
}
