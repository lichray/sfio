#include	"sfstdio.h"

/*	Internal scanf egine to read from stdin
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int vscanf(const char* form, va_list args)
#else
int vscanf(form,args)
char    *form;          /* format to use */
va_list args;           /* arg list if argf == 0 */
#endif
{
	reg int		rv;
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(stdin)))
		return -1;
	_stdclrerr(stdin,sp);
	if((rv = sfvscanf(sp,form,args)) <= 0)
	{	if(sfeof(sp))
			_stdeof(stdin);
		if(sferror(sp))
			_stderr(stdin);
	}

	return rv;
}
