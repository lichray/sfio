#include	"sfstdio.h"

/*	Write out a word
**	Written by Kiem-Phong Vo
*/


#if __STD_C
int putw( int c, FILE *fp)
#else
int putw(c, fp)
int	c;
reg FILE *fp;
#endif
{
	reg Sfio_t	*sp;

	if(!(sp = _sfstream(fp)))
		return -1;
	_stdclrerr(fp,sp);
	if(sfwrite(sp,(char*)(&c),sizeof(int)) <= 0)
	{	_stderr(fp);
		return(1);
	}
	return(0);
}
