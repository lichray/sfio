#include	"sfstdio.h"

/*	Reposition stream IO pointer
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int fseek(reg FILE* fp, long offset, int whence)
#else
int fseek(fp,offset,whence)
reg FILE*	fp;
reg long	offset;
reg int		whence;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);

	/* ready for either read or write */
#if _FILE_cnt
	fp->std_cnt = 0;
#endif
#if _FILE_r
	fp->std_r = 0;
#endif
#if _FILE_w
	fp->std_w = 0;
#endif
#if _FILE_readptr
	fp->std_readptr = fp->std_readend = NIL(uchar*);
#endif
#if _FILE_writeptr
	fp->std_writeptr = fp->std_writeend = NIL(uchar*);
#endif

	return sfseek(sp,(Sfoff_t)offset,whence) < 0L ? -1 : 0;
}
