#include	"sfhdr.h"

#if __STD_C
static int __sferror(reg Sfio_t* f)
#else
static int __sferror(f)
reg Sfio_t	*f;
#endif
{
	return sferror(f);
}

#undef sferror

#if __STD_C
int sferror(reg Sfio_t* f)
#else
int sferror(f)
reg Sfio_t	*f;
#endif
{
	return __sferror(f);
}
