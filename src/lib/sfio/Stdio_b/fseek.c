#include	"sfstdio.h"

/*	Reposition stream IO pointer
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int fseek(reg FILE *fp, long offset, int whence)
#else
int fseek(fp,offset,whence)
reg FILE	*fp;
reg long	offset;
reg int		whence;
#endif
{
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
#if _FILE_cnt
	/* ready for either read or write */
	fp->std_cnt = 0;
#endif
	return sfseek(sp,offset,whence) < 0L ? -1 : 0;
}
