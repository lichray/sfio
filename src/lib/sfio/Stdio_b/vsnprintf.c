#include	"sfstdio.h"

/*	Internal printf engine to write to a string buffer
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int vsnprintf(char* s, int n, const char* form, va_list args)
#else
int vsnprintf(s,n,form,args)
reg char*	s;
reg int		n;
reg char*	form;
va_list		args;
#endif
{
	Sfio_t	f;
	reg int	rv;

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

	return rv;
}
