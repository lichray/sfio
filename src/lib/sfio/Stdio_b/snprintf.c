#include	"sfstdio.h"

/*	Format a string with given buffer size.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int snprintf(char *s, int n,  const char *form, ...)
#else
int snprintf(va_alist)
va_dcl
#endif
{
	va_list		args;
	Sfio_t		f;
	reg int		rv;
#if __STD_C
	va_start(args,form);
#else
	reg char	*s;
	reg int		n;
	reg char	*form;
	va_start(args);
	s = va_arg(args,char*);
	n = va_arg(args,int);
	form = va_arg(args,char*);
#endif

	if(!s)
		return -1;

	/* make a fake stream */
	SFCLEAR(&f);
	f.flags = SF_STRING|SF_WRITE;
	f.mode = SF_WRITE;
	f.size = n;
	f.data = f.next = f.endr = (uchar*)s;
	f.endb = f.endw = f.data+f.size;

	rv = sfvprintf(&f,form,args);
	*f.next = '\0';
	_Sfi = f.next - f.data;

	va_end(args);

	return rv;
}
