#include	"sfstdio.h"

/*	Write out a string to stdout.
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int puts(const char* str)
#else
int puts(str)
reg char*	str;
#endif
{
	reg int		rv;
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(stdout)))
		return -1;
	_stdclrerr(stdout,sp);
	if((rv = sfputr(sfstdout,str,'\n')) < 0)
		_stderr(stdout);
	return rv;	
}
