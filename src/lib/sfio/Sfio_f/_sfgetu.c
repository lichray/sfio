#include	"sfhdr.h"

#if __STD_C
static ulong __sfgetu(reg Sfio_t* f)
#else
static ulong __sfgetu(f)
reg Sfio_t	*f;
#endif
{
	return sfgetu(f);
}

#undef sfgetu

#if __STD_C
ulong sfgetu(reg Sfio_t* f)
#else
ulong sfgetu(f)
reg Sfio_t	*f;
#endif
{
	return __sfgetu(f);
}
