#include	"sfhdr.h"

#if __STD_C
static int __sfstacked(reg Sfio_t* f)
#else
static int __sfstacked(f)
reg Sfio_t	*f;
#endif
{
	return sfstacked(f);
}

#undef sfstacked

#if __STD_C
int sfstacked(reg Sfio_t* f)
#else
int sfstacked(f)
reg Sfio_t	*f;
#endif
{
	return __sfstacked(f);
}
