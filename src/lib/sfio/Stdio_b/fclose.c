#include	"sfstdio.h"

/*	Closing a stream.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fclose(reg FILE* f)
#else
int fclose(f)
FILE*	f;
#endif
{
	reg int		rv;
	reg Sfio_t*	sf;

	if(!(sf = SFSTREAM(f)))
		return -1;

	if((rv = sfclose(sf)) >= 0 && f != stdin && f != stdout && f != stderr)
		free(f);
	return rv;
}
