#include	"sfhdr.h"

#if __STD_C
static int __sfgetc(reg Sfio_t* f)
#else
static int __sfgetc(f)
reg Sfio_t	*f;
#endif
{
	return sfgetc(f);
}

#undef sfgetc

#if __STD_C
int sfgetc(reg Sfio_t* f)
#else
int sfgetc(f)
reg Sfio_t	*f;
#endif
{
	return __sfgetc(f);
}
