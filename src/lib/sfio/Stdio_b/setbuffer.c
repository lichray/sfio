#include	"sfstdio.h"

/*	Change stream buffer
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int setbuffer(reg FILE *fp, char* buf, int size)
#else
int setbuffer(fp,buf, size)
reg FILE *fp;
char	*buf;
int	size;
#endif
{
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	sfsetbuf(sp,(Void_t*)buf, buf ? 0 : size);
	return 0;
}
