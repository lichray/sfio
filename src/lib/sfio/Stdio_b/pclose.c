#include	"sfstdio.h"

/*	Closing a popen stream
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int pclose(reg FILE* fp)
#else
int pclose(fp)
reg FILE*	fp;
#endif
{
	return fclose(fp);
}
