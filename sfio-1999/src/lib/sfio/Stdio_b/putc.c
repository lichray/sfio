#include	"sfstdio.h"

/*	Write out a byte
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int putc(int c, FILE* fp)
#else
int putc(c, fp)
reg int		c;
reg FILE*	fp;
#endif
{
	return fputc(c,fp);
}
