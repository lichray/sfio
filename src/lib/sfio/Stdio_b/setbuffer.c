#include	"sfstdio.h"

/*	Change stream buffer
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int setbuffer(reg FILE* fp, char* buf, size_t size)
#else
int setbuffer(fp,buf, size)
reg FILE*	fp;
char*		buf;
size_t		size;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	sfsetbuf(sp,(Void_t*)buf, buf ? size : 0);
	return 0;
}
