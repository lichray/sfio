#include	"sfstdio.h"

/*	Fill buffer.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int _filbuf(FILE *fp)
#else
int _filbuf(fp)
reg FILE	*fp;
#endif
{
	reg Sfio_t	*sp;
	reg int		rv;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfgetc(sp)) < 0)
	{
		if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
	}
#if _FILE_cnt
	else
	{	/* let user have fast access to the data */
		fp->std_ptr = sp->next;
		fp->std_cnt = sp->endb-sp->next;

		/* protection against mixing sfio/stdio */
		_Sfstdio = _sfstdio;
		sp->mode |= SF_STDIO;
		sp->endr = sp->endw = sp->data;
	}
#endif

	return(rv);
}
