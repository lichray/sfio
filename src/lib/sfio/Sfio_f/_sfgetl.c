#include	"sfhdr.h"

#if __STD_C
static long __sfgetl(reg Sfio_t* f)
#else
static long __sfgetl(f)
reg Sfio_t	*f;
#endif
{
	return sfgetl(f);
}

#undef sfgetl

#if __STD_C
long sfgetl(reg Sfio_t* f)
#else
long sfgetl(f)
reg Sfio_t	*f;
#endif
{
	return __sfgetl(f);
}
