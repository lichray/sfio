#include	"sfstdio.h"

/*	Change buffer and set/unset line buffering.
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int setvbuf(reg FILE* fp, char* buf, int flags, size_t size)
#else
int setvbuf(fp, buf, flags, size)
reg FILE*	fp;
char*		buf;
int		flags;
size_t		size;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);

	if(flags == _IOLBF)
		sfset(sp,SF_LINE,1);
	else if(flags == _IONBF)
	{	sfsync(sp);
		sfsetbuf(sp,NIL(Void_t*),0);
	}
	else if(flags == _IOFBF)
	{	if(size == 0)
			size = BUFSIZ;
		sfsync(sp);
		sfsetbuf(sp,buf,size);
	}
	return(0);
}
