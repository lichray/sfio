#include	"sfstdio.h"

/*	Write out a set of data
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fwrite(const void *buf, int size, int nmem, reg FILE *fp)
#else
int fwrite(buf,size,nmem,fp)
reg char	*buf;
reg int		size;
reg int		nmem;
reg FILE	*fp;
#endif
{
	reg int		rv;
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);

	if((rv = sfwrite(sp,(char*)buf,size*nmem)) < 0)
		_stderr(fp);
	else	rv /= size;

	return(rv);
}
