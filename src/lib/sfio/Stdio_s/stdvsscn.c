#include	"sfhdr.h"

#if __STD_C
int _stdvsscanf(char *s, const char *form, va_list args)
#else
int _stdvsscanf(s,form,args)
register char	*s;
register char	*form;
va_list		args;
#endif
{
	Sfio_t	f;

	if(!s)
		return -1;

	/* make a fake stream */
	SFCLEAR(&f);
	f.flags = SF_STRING|SF_READ;
	f.mode = SF_READ;
	f.size = strlen((char*)s);
	f.data = f.next = f.endr = (uchar*)s;
	f.endb = f.endw = f.data+f.size;

	return sfvscanf(&f,form,args);
}
