#include	"sfstdio.h"

/*	Scan data from a string
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int sscanf(const char *s, const char *form,...)
#else
int sscanf(va_alist)
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
	reg char	*form;
	va_start(args);
	s = va_arg(args,char*);
	form = va_arg(args,char*);
#endif

	if(!s)
		return -1;

	/* make a fake stream */
	SFCLEAR(&f);
	f.flags = SF_STRING|SF_READ;
	f.mode = SF_READ;
	f.size = strlen((char*)s);
	f.data = f.next = f.endw = (uchar*)s;
	f.endb = f.endr = f.data+f.size;
	sfset(&f,f.flags,1);

	rv = sfvscanf(&f,form,args);
	va_end(args);

	return rv;
}
