#include	"sfhdr.h"

#if __STD_C
static int __sfputd(reg Sfio_t* f, reg double v)
#else
static int __sfputd(f,v)
reg Sfio_t	*f;
reg double	v;
#endif
{
	return sfputd(f,v);
}

#undef sfputd

#if __STD_C
int sfputd(reg Sfio_t* f, reg double v)
#else
int sfputd(f,v)
reg Sfio_t	*f;
reg double	v;
#endif
{
	return __sfputd(f,v);
}
