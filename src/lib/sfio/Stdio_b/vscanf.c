#include	"sfstdio.h"

/*	Internal scanf egine to read from stdin
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int vscanf(const char* form, va_list args)
#else
int vscanf(form,args)
char*	form;          /* format to use */
va_list args;           /* arg list if argf == 0 */
#endif
{
	reg int		rv;
	reg Sfio_t*	sf;

	if(!(sf = SFSTREAM(stdin)))
		return -1;

	if((rv = sfvscanf(sf,form,args)) <= 0)
		_stdseterr(stdin,sf);

	return rv;
}
