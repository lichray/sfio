#include	"sfstdio.h"

/*	Get a byte
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int getc(reg FILE *fp)
#else
int getc(fp)
reg FILE *fp;
#endif
{
	return fgetc(fp);
}
