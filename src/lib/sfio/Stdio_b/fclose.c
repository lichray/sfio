#include	"sfstdio.h"

/*	Closing a stream.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fclose(reg FILE *fp)
#else
int fclose(fp)
reg FILE	*fp;
#endif
{
	reg int		rv;
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfclose(sp)) >= 0)
		_stdclose(fp);
	return rv;
}
