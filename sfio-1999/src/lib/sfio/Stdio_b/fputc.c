#include	"sfstdio.h"

/*	Write out a byte
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int fputc(int c, FILE* fp)
#else
int fputc(c, fp)
reg int		c;
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if(sfputc(sp,c) < 0)
	{	_stderr(fp);
		return -1;
	}
	else	return c;
}

#if _lib__IO_putc
#if __STD_C
int _IO_putc(int c, FILE* fp)
#else
int _IO_putc(c, fp)
reg int		c;
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if(sfputc(sp,c) < 0)
	{	_stderr(fp);
		return -1;
	}
	else	return c;
}
#endif
