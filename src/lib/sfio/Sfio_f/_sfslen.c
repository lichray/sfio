#include	"sfhdr.h"

#if __STD_C
static int __sfslen(void)
#else
static int __sfslen()
#endif
{
	return sfslen();
}

#undef sfslen

#if __STD_C
int sfslen(void)
#else
int sfslen()
#endif
{
	return __sfslen();
}
