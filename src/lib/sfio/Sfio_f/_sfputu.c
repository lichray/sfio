#include	"sfhdr.h"

#if __STD_C
static int __sfputu(reg Sfio_t* f, reg ulong v)
#else
static int __sfputu(f,v)
reg Sfio_t	*f;
reg ulong	v;
#endif
{
	return sfputu(f,v);
}

#undef sfputu

#if __STD_C
int sfputu(reg Sfio_t* f, reg ulong v)
#else
int sfputu(f,v)
reg Sfio_t	*f;
reg ulong	v;
#endif
{
	return __sfputu(f,v);
}
