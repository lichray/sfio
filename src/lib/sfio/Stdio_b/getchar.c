#include	"sfstdio.h"


/*	Get a byte from stdin.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int getchar(void)
#else
int getchar()
#endif
{
	return fgetc(stdin);
}
