#include	"sfhdr.h"
#include	"stdio.h"
#undef putc

#if __STD_C
int _stdfputc(int c, FILE* f)
#else
int _stdfputc(c, f)
int	c;
FILE*	f;
#endif
{
	return sfputc(f,c);
}
