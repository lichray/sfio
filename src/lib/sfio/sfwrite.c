#include	"sfhdr.h"

/*	Write data out to the file system
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
ssize_t sfwrite(reg Sfio_t* f, const Void_t* buf, reg size_t n)
#else
ssize_t sfwrite(f,buf,n)
reg Sfio_t*	f;	/* write to this stream. 	*/
Void_t*		buf;	/* buffer to be written.	*/
reg size_t	n;	/* number of bytes. 		*/
#endif
{
	reg uchar	*s, *begs, *next;
	reg ssize_t	w;
	reg int		local, justseek;

	GETLOCAL(f,local);
	justseek = f->bits&SF_JUSTSEEK; f->bits &= ~SF_JUSTSEEK;

	/* release peek lock */
	if(f->mode&SF_PEEK)
	{	if(!(f->mode&SF_WRITE) && (f->flags&SF_RDWR) != SF_RDWR)
			return -1;

		if((uchar*)buf != f->next)
		{	reg Sfrsrv_t*	frs = _sfrsrv(f,0);
			if((uchar*)buf != frs->data)
				return -1;
		}

		f->mode &= ~SF_PEEK;

		if(f->mode&SF_PKRD)
		{	/* read past peeked data */
			char		buf[16];
			reg ssize_t	r;

			for(w = n; w > 0; )
			{	if((r = w) > sizeof(buf))
					r = sizeof(buf);
				if((r = read(f->file,buf,r)) <= 0)
				{	n -= w;
					break;
				}
				else	w -= r;
			}

			f->mode &= ~SF_PKRD;
			f->endb = f->data + n;
			f->here += n;
		}

		if((f->mode&SF_READ) && (f->bits&SF_PROCESS))
			f->next += n;
	}

	s = begs = (uchar*)buf;
	for(;; f->mode &= ~SF_LOCK)
	{	/* check stream mode */
		if(SFMODE(f,local) != SF_WRITE && _sfmode(f,SF_WRITE,local) < 0 )
			return s > begs ? s-begs : -1;

		if(n <= 0)
			break;

		SFLOCK(f,local);

		w = f->endb - f->next;
		if(f->next == s) /* fast write after sfreserve */	
		{	if(w > (ssize_t)n )
				w = (ssize_t)n;
			f->next = (s += w);
			n -= w;
			break;
		}
		else if((w > 0 && w < (ssize_t)n && (f->flags&SF_WHOLE) ) ||
			(w == 0 && (f->next > f->data || (f->flags&SF_STRING)) ) )
		{	if(SFFLSBUF(f,-1) < 0)
				break;
			w = f->endb - f->next;
		}

		if(!(f->flags&SF_STRING) && f->next == f->data &&
		   ((ssize_t)n >= f->size ||
		    ((f->flags&SF_WHOLE) && (ssize_t)n > w) ||
		    (justseek && SFUNBUFFER(f,n)) ) )
		{	if((w = SFWR(f,s,n,f->disc)) <= 0)
				break;
			s += w;
		}
		else
		{	if(w > (ssize_t)n)
				w = (ssize_t)n;
			next = f->next;
			MEMCPY(next,s,w);
			f->next = next;
		}

		if((n -= w) <= 0)
			break;
	}

	/* always flush buffer for share streams */
	if(f->extent < 0 && (f->flags&SF_SHARE) && !(f->flags&SF_PUBLIC) )
		(void)SFFLSBUF(f,-1);

	/* check to see if buffer should be flushed */
	else if(n == 0 && (f->flags&SF_LINE) && !(f->flags&SF_STRING))
	{	if((ssize_t)(n = f->next-f->data) > (w = s-begs))
			n = w;
		if(n > 0 && n < HIFORLINE)
		{	for(next = f->next-1; n > 0; --n, --next)
			{	if(*next == '\n')
				{	n = HIFORLINE;
					break;
				}
			}
		}
		if(n >= HIFORLINE)
			(void)SFFLSBUF(f,-1);
	}

	SFOPEN(f,local);

	return s-begs;
}
