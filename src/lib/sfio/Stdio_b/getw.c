#include	"sfstdio.h"

/*	Get a word.
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int getw(FILE* fp)
#else
int getw(fp)
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;
	int		w;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if(sfread(sp,(char*)(&w),sizeof(int)) != sizeof(int))
	{
		if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
		return -1;
	}

	return w;
}
