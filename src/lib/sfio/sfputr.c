#include	"sfhdr.h"

/*	Put out a nul-terminated string
**	Note that the reg declarations below must be kept in
**	their relative order so that the code will configured
**	correctly on Vaxes to use "asm()".
**
**	Written by Kiem-Phong Vo
*/
#if __STD_C
ssize_t sfputr(reg Sfio_t* f, const char* s, reg int rc)
#else
ssize_t sfputr(f,s,rc)
reg Sfio_t*	f;	/* write to this stream	*/
char*		s;	/* string to write	*/
reg int		rc;	/* record separator.	*/
#endif
{
	reg ssize_t	p, n;
	reg uchar	*os, *ps;

	if(f->mode != SF_WRITE && _sfmode(f,SF_WRITE,0) < 0)
		return -1;

	SFLOCK(f,0);
	os = (uchar*)s;
	if(f->size <= 0)
	{	/* unbuffered stream */
		n = strlen((char*)os);
		if((p = SFWRITE(f,(Void_t*)os,n)) > 0)
			os += p;
		goto done;
	}

	while(*os)
	{	/* peek buffer for space */
		if(SFWPEEK(f,ps,p) <= 0)
			break;
#if _lib_memccpy
		if((ps = (uchar*)memccpy(ps,os,'\0',p)) != NIL(uchar*))
			ps -= 1;
		else	ps  = f->next+p;
		os += ps - f->next;
#else
		/* fast copy loop */
		while((*ps++ = *os++) != '\0' && --p > 0)
			;
		if(*--ps != 0)
			ps += 1;
		else	os -= 1;
#endif
		f->next = ps;
	}

done:
	p = (char*)os - (char*)s;
	if(rc >= 0)
	{	if(f->next >= f->endb)
			(void)SFFLSBUF(f,(int)((uchar)rc));
		else	*f->next++ = (uchar)rc;
		p += 1;
	}

	/* sync unseekable shared streams */
	if(f->extent < 0 && (f->flags&SF_SHARE) )
		(void)SFFLSBUF(f,-1);

	/* check for line buffering */
	else if((f->flags&SF_LINE) && !(f->flags&SF_STRING) && (n = f->next-f->data) > 0)
	{	if(n > p)
			n = p;
		f->next -= n;
		(void)SFWRITE(f,(Void_t*)f->next,n);
	}

	SFOPEN(f,0);

	return p;
}
