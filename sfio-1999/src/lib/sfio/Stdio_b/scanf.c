#include	"sfstdio.h"

/*	Read input from stdin using a given format
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int scanf(const char* form, ...)
#else
int scanf(va_alist)
va_dcl
#endif
{
	va_list		args;
	reg int		rv;
	reg Sfio_t*	sp;
#if __STD_C
	va_start(args,form);
#else
	reg char*	form;	/* scanning format */
	va_start(args);
	form = va_arg(args,char*);
#endif

	if(!(sp = _sfstream(stdin)))
		return -1;
	_stdclrerr(stdin,sp);

	rv = sfvscanf(sp,form,args);
	va_end(args);

	if(rv <= 0)
	{	if(sfeof(sp))
			_stdeof(stdin);
		if(sferror(sp))
			_stderr(stdin);
	}

	return rv;
}
