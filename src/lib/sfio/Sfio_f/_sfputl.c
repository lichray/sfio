#include	"sfhdr.h"

#if __STD_C
static int __sfputl(reg Sfio_t* f, reg long v)
#else
static int __sfputl(f,v)
reg Sfio_t	*f;
reg long	v;
#endif
{
	return sfputl(f,v);
}

#undef sfputl

#if __STD_C
int sfputl(reg Sfio_t* f, reg long v)
#else
int sfputl(f,v)
reg Sfio_t	*f;
reg long	v;
#endif
{
	return __sfputl(f,v);
}
