#include	"sfhdr.h"

#if __STD_C
static int __sfputc(reg Sfio_t* f, reg int c)
#else
static int __sfputc(f,c)
reg Sfio_t	*f;
reg int		c;
#endif
{
	return sfputc(f,c);
}

#undef sfputc

#if __STD_C
int sfputc(reg Sfio_t* f, reg int c)
#else
int sfputc(f,c)
reg Sfio_t	*f;
reg int		c;
#endif
{
	return __sfputc(f,c);
}
