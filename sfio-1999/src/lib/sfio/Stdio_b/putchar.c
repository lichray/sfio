#include	"sfstdio.h"

/*	Write out a byte to stdout.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int putchar(int c)
#else
int putchar(c)
reg int	c;
#endif
{
	return fputc(c,stdout);
}
