#include	"sfstdio.h"

/*	Flush output buffer.
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int _flsbuf(int c, FILE *fp)
#else
int _flsbuf(c, fp)
reg int		c;
reg FILE	*fp;
#endif
{
	reg Sfio_t *sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if(sfputc(sp,c) < 0)
	{	_stderr(fp);
		return -1;
	}

	/* fast access to buffer for putc benefit */
#if _FILE_cnt
	if(!(sp->flags&SF_LINE))
	{	fp->std_ptr = sp->next;
		fp->std_cnt = sp->endb - sp->next;

		/* internal protection against mixing of sfio/stdio */
		_Sfstdio = _sfstdio;
		sp->mode |= SF_STDIO;
		sp->endr = sp->endw = sp->data;
	}
#endif

	return c;
}
