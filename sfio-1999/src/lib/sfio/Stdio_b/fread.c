#include	"sfstdio.h"

/*	Read a set of data
**	Written by Kiem-Phong Vo
*/

#if __STD_C
size_t fread(void* buf, size_t size, size_t nmem, reg FILE* fp)
#else
size_t fread(buf,size,nmem,fp)
reg char*	buf;
reg size_t	size;
reg size_t	nmem;
reg FILE*	fp;
#endif
{
	reg Sfio_t*	sp;
	reg ssize_t	rv;

	if(!(sp = _sfstream(fp)))
		return 0;

	_stdclrerr(fp,sp);
	if((rv = sfread(sp,buf,size*nmem)) > 0)
		return rv/size;
	else
	{	if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
		return 0;
	}
}
