#include	"sfstdio.h"

/*	The internal printf engine.
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int _doprnt(const char* form, va_list args, FILE* fp)
#else
int _doprnt(form,args,fp)
char    *form;          /* format to use */
va_list args;           /* arg list if argf == 0 */
FILE	*fp;
#endif
{
	reg int		rv;
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;

	_stdclrerr(fp,sp);
	if((rv = sfvprintf(sp,form,args)) < 0)
		_stderr(fp);
	return rv;
}
