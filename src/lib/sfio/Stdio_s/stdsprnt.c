#include	"sfhdr.h"
#include	"stdio.h"


/*	sprintf function
**
**	Written by Kiem-Phong Vo (12/10/90)
*/


#if __STD_C
int _stdsprintf(char *s, const char *form, ...)
#else
int _stdsprintf(va_alist)
va_dcl
#endif
{
	va_list	args;
	Sfio_t	f;
	reg int	rv;

#if __STD_C
	va_start(args,form);
#else
	reg char	*s;
	reg char	*form;
	va_start(args);
	s = va_arg(args,char*);
	form = va_arg(args,char*);
#endif

	if(!s)
		return -1;

	/* make a fake stream */
	SFCLEAR(&f);
	f.flags = SF_STRING|SF_WRITE;
	f.mode = SF_WRITE;
	f.size = 4*SF_BUFSIZE;
	f.data = f.next = f.endr = (uchar*)s;
	f.endb = f.endw = f.data+f.size;

	rv = sfvprintf(&f,form,args);
	*f.next = '\0';
	_Sfi = f.next - f.data;

	va_end(args);

	return rv;
}
