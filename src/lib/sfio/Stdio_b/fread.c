#include	"sfstdio.h"

/*	Read a set of data
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fread(void *buf, int size, int nmem, reg FILE *fp)
#else
int fread(buf,size,nmem,fp)
reg char	*buf;
reg int		size;
reg int		nmem;
reg FILE	*fp;
#endif
{
	reg Sfio_t	*sp;
	reg int		rv;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfread(sp,(char*)buf,size*nmem)) <= 0)
	{	if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
	}
	else	rv /= size;

	return(rv);
}
