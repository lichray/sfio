#include	"sfstdio.h"

/*	Get a string from stdin.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
char *gets(char *buf)
#else
char *gets(buf)
char	*buf;
#endif
{
	reg Sfio_t	*sp;
	reg char	*rv;

	if(!(sp = _sfstream(stdin)))
		return NIL(char*);

	_stdclrerr(stdin,sp);
	if(!(rv = _stdgets(sp,buf,BUFSIZ,1)))
	{	if(sfeof(sp))
			_stdeof(stdin);
		if(sferror(sp))
			_stderr(stdin);
	}
	return rv;
}
