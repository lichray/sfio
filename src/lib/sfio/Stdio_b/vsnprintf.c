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
	return (s && form) ? sfvsprintf(s,n,form,args) : -1;
}

#if _lib___vsnprintf && !done_lib___vsnprintf && !defined(vsnprintf)
#define done_lib___vsnprintf 1
#define vsnprintf __vsnprintf
#include        "vsnprintf.c"
#endif

