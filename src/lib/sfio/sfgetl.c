#include	"sfhdr.h"

/*	Read a long value coded in a portable format.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
long _sfgetl(reg Sfio_t* f)
#else
long _sfgetl(f)
reg Sfio_t	*f;
#endif
{
	reg uchar	*s, *ends, c;
	reg int		p;
	reg long	v;

	if(!(_Sfi&SF_MORE))	/* must be a small negative number */
		return -SFSVALUE(_Sfi)-1;

	SFLOCK(f,0);
	v = SFUVALUE(_Sfi);
	for(;;)
	{
		if(SFRPEEK(f,s,p) <= 0)
		{	f->flags |= SF_ERROR;
			v = -1L;
			goto done;
		}
		for(ends = s+p; s < ends;)
		{
			c = *s++;
			if(c&SF_MORE)
				v = ((ulong)v << SF_UBITS) | SFUVALUE(c);
			else
			{	/* special translation for this byte */
				v = ((ulong)v << SF_SBITS) | SFSVALUE(c);
				f->next = s;
				v = (c&SF_SIGN) ? -v-1 : v;
				goto done;
			}
		}
		f->next = s;
	}
done :
	SFOPEN(f,0);
	return v;
}
