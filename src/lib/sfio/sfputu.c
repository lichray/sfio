#include	"sfhdr.h"

/*	Write out an unsigned long value in a portable format.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
int _sfputu(reg Sfio_t* f, reg ulong v)
#else
int _sfputu(f,v)
reg Sfio_t	*f;	/* write a portable ulong to this stream */
reg ulong	v;	/* the unsigned value to be written */
#endif
{
#define N_ARRAY		(2*sizeof(long))
	reg uchar	*s, *ps;
	reg int		n, p;
	uchar		c[N_ARRAY];

	if(f->mode != SF_WRITE && _sfmode(f,SF_WRITE,0) < 0)
		return -1;
	SFLOCK(f,0);

	/* code v as integers in base SF_UBASE */
	s = ps = &(c[N_ARRAY-1]);
	*s = (uchar)SFUVALUE(v);
	while((v >>= SF_UBITS) )
		*--s = (uchar)(SFUVALUE(v) | SF_MORE);
	n = (ps-s)+1;

	if(n > 4 || SFWPEEK(f,ps,p) < n)
		n = SFWRITE(f,(Void_t*)s,n); /* write the hard way */
	else
	{	switch(n)
		{
		case 4 : *ps++ = *s++;
		case 3 : *ps++ = *s++;
		case 2 : *ps++ = *s++;
		case 1 : *ps++ = *s++;
		}
		f->next = ps;
	}

	SFOPEN(f,0);
	return n;
}
