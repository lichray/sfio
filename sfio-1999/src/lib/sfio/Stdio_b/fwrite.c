#include	"sfstdio.h"

/*	Write out a set of data
**	Written by Kiem-Phong Vo
*/

#if __STD_C
size_t fwrite(const void* buf, size_t size, size_t nmem, reg FILE* fp)
#else
size_t fwrite(buf,size,nmem,fp)
reg char*	buf;
reg size_t	size;
reg size_t	nmem;
reg FILE*	fp;
#endif
{
	reg ssize_t	rv;
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return 0;

	_stdclrerr(fp,sp);

	if((rv = sfwrite(sp,buf,size*nmem)) >= 0)
		return rv/size;
	else
	{	_stderr(fp);
		return 0;
	}
}
