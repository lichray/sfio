#include	"sfstdio.h"

/*	Get a byte.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fgetc(reg FILE* fp)
#else
int fgetc(fp)
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;
	reg int		rv;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfgetc(sp)) < 0)
	{	if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
	}
	return rv;
}

#if _lib__IO_getc
#if __STD_C
int _IO_getc(reg FILE* fp)
#else
int _IO_getc(fp)
reg FILE* fp;
#endif
{
	reg Sfio_t*	sp;
	reg int		rv;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfgetc(sp)) < 0)
	{	if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
	}
	return rv;
}
#endif
