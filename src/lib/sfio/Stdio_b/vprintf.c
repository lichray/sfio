#include	"sfstdio.h"

/*	Internal printf engine to write to stdout
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int vprintf(const char* form, va_list args)
#else
int vprintf(form,args)
char*	form;          /* format to use */
va_list args;           /* arg list if argf == 0 */
#endif
{
	reg int		rv;
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(stdout)))
		return -1;

	_stdclrerr(stdout,sp);
	if((rv = sfvprintf(sp,form,args)) < 0)
		_stderr(stdout);

	return rv;
}
