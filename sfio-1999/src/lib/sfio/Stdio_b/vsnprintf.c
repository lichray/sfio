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
	return sfvsprintf(s,n,form,args);
}
