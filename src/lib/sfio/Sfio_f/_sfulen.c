#include	"sfhdr.h"

#if __STD_C
static int __sfulen(reg ulong v)
#else
static int __sfulen(v)
reg ulong	v;
#endif
{
	return sfulen(v);
}

#undef sfulen

#if __STD_C
int sfulen(reg ulong v)
#else
int sfulen(v)
reg ulong	v;
#endif
{
	return __sfulen(v);
}
