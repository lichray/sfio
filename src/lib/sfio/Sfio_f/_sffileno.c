#include	"sfhdr.h"

#if __STD_C
static int __sffileno(reg Sfio_t* f)
#else
static int __sffileno(f)
reg Sfio_t	*f;
#endif
{
	return sffileno(f);
}

#undef sffileno

#if __STD_C
int sffileno(reg Sfio_t* f)
#else
int sffileno(f)
reg Sfio_t	*f;
#endif
{
	return __sffileno(f);
}
