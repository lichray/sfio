#include	"sfhdr.h"

#if __STD_C
static int __sfeof(reg Sfio_t* f)
#else
static int __sfeof(f)
reg Sfio_t	*f;
#endif
{
	return sfeof(f);
}

#undef sfeof

#if __STD_C
int sfeof(reg Sfio_t* f)
#else
int sfeof(f)
reg Sfio_t	*f;
#endif
{
	return __sfeof(f);
}
