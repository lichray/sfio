#include	"sfstdio.h"

/*	The internal scanf engine.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int _doscan(FILE *fp, const char* form, va_list args)
#else
int _doscan(fp,form,args)
FILE	*fp;
char    *form;          /* format to use */
va_list args;           /* arg list if argf == 0 */
#endif
{
	reg int		rv;
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfvscanf(sp,form,args)) <= 0)
	{	if(sfeof(sp))
			_stdeof(fp);
		if(sferror(sp))
			_stderr(fp);
	}
	return rv;
}
