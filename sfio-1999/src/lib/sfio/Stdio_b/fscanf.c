#include	"sfstdio.h"

/*	Scan input data using a given format
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fscanf(FILE* fp, const char* form, ...)
#else
int fscanf(va_alist)
va_dcl
#endif
{
	va_list		args;
	reg int		rv;
	reg Sfio_t*	sp;

#if __STD_C
	va_start(args,form);
#else
	reg FILE*	fp;	/* file to be scanned */
	reg char*	form;	/* scanning format */
	va_start(args);
	fp = va_arg(args,FILE*);
	form = va_arg(args,char*);
#endif

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);

	rv = sfvscanf(sp,form,args);

	va_end(args);

	if(rv <= 0)
	{
		if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
	}

	return rv;
}
