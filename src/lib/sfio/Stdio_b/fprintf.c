#include	"sfstdio.h"

/*	Format data.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fprintf(FILE *fp, const char *form, ...)
#else
int fprintf(va_alist)
va_dcl
#endif
{
	va_list		args;
	reg int		rv;
	reg Sfio_t	*sp;

#if __STD_C
	va_start(args,form);
#else
	reg FILE	*fp;	/* file stream to print to */
	reg char	*form;	/* print format */
	va_start(args);
	fp = va_arg(args,FILE*);
	form = va_arg(args,char*);
#endif

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	rv = sfvprintf(sp,form,args);

	va_end(args);

	if(rv < 0)
		_stderr(fp);

	return rv;
}
