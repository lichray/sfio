#include	"sfstdio.h"

/*	Change stream buffer
**	Written by Kiem-Phong Vo
*/


#if __STD_C
void setbuf(reg FILE* fp, char* buf)
#else
void setbuf(fp,buf)
reg FILE*	fp;
char*		buf;
#endif
{
	reg Sfio_t*	sp;

	if(!(sp = _sfstream(fp)))
		return;
	_stdclrerr(fp,sp);
	(void)sfsetbuf(sp,(Void_t*)buf,BUFSIZ);
}
