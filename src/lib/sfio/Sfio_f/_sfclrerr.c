#include	"sfhdr.h"

#if __STD_C
static int __sfclrerr(reg Sfio_t* f)
#else
static int __sfclrerr(f)
reg Sfio_t	*f;
#endif
{
	return sfclrerr(f);
}

#undef sfclrerr

#if __STD_C
int sfclrerr(reg Sfio_t* f)
#else
int sfclrerr(f)
reg Sfio_t	*f;
#endif
{
	return __sfclrerr(f);
}
