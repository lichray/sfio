#include	"sfhdr.h"
#include	"stdio.h"

/*	Write out a block of data
**
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
ssize_t _stdfwrite(const Void_t* buf, size_t esize, size_t nelts, Sfio_t* f)
#else
ssize_t _stdfwrite(buf, esize, nelts, f)
Void_t*	buf;
size_t	esize;
size_t	nelts;
Sfio_t*	f;
#endif
{
	ssize_t	rv;

	if((rv = sfwrite(f, buf, esize*nelts)) > 0)
		return rv/esize;
	else	return 0;
}
