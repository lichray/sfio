#include	"sfstdio.h"

/*	Format a string with given buffer size.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int snprintf(char* s, int n,  const char* form, ...)
#else
int snprintf(va_alist)
va_dcl
#endif
{
	va_list		args;
	reg int		rv;
#if __STD_C
	va_start(args,form);
#else
	reg char*	s;
	reg int		n;
	reg char*	form;
	va_start(args);
	s = va_arg(args,char*);
	n = va_arg(args,int);
	form = va_arg(args,char*);
#endif

	rv = s ? sfvsprintf(s,n,form,args) : -1;
	va_end(args);

	return rv;
}
