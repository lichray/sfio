#include	"sfhdr.h"

/*	Read n bytes from a stream into a buffer
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
ssize_t sfread(reg Sfio_t* f, Void_t* buf, reg size_t n)
#else
ssize_t sfread(f,buf,n)
reg Sfio_t*	f;	/* read from this stream. 	*/
Void_t*		buf;	/* buffer to read into		*/
reg size_t	n;	/* number of bytes to be read. 	*/
#endif
{
	reg uchar	*s, *begs, *next;
	reg ssize_t	r;
	reg int		local, justseek;

	GETLOCAL(f,local);
	justseek = f->bits&SF_JUSTSEEK; f->bits &= ~SF_JUSTSEEK;

	/* release peek lock */
	if(f->mode&SF_PEEK)
	{	if(!(f->mode&SF_READ) )
			return -1;

		if(f->mode&SF_GETR)
		{	if(((uchar*)buf + f->val) != f->next)
			{	Sfrsrv_t* frs;
				if(!(frs = _sfrsrv(f,0)) || frs->data != (uchar*)buf)
					return -1;
			}
			f->mode &= ~SF_PEEK;
			return 0;
		}
		else
		{	if((uchar*)buf != f->next)
				return -1;
			f->mode &= ~SF_PEEK;
			if(f->mode&SF_PKRD)
			{	/* actually read the data now */
				f->mode &= ~SF_PKRD;
				if(n > 0)
					n = (r = read(f->file,f->data,n)) < 0 ? 0 : r;
				f->endb = f->data+n;
				f->here += n;
			}
			f->next += n;
			f->endr = f->endb;
			return n;
		}
	}

	s = begs = (uchar*)buf;
	for(;; f->mode &= ~SF_LOCK)
	{	/* check stream mode */
		if(SFMODE(f,local) != SF_READ && _sfmode(f,SF_READ,local) < 0)
			return s > begs ? s-begs : -1;

		if(n <= 0)
			break;

		SFLOCK(f,local);

		/* determine current amount of buffered data */
		if((r = f->endb - f->next) > (ssize_t)n)
			r = n;

		if(r > 0)
		{	/* copy buffered data */
			if((next = f->next) == s)
				f->next += r;
			else
			{	MEMCPY(s,next,r);
				f->next = next;
			}
			if((n -= r) <= 0)
				break;
		}
		else if((f->flags&SF_STRING) || (f->bits&SF_MMAP) )
		{	if(SFFILBUF(f,-1) <= 0)
				break;
		}
		else
		{	/* reset buffer before refilling */
			f->next = f->endb = f->data;

			if(justseek) /* limit buffer filling */
			{	if(SFUNBUFFER(f,n) ||
				   (r = (n/SF_GRAIN + 1)*SF_GRAIN) > f->size )
					r = (ssize_t)n;
			}
			else if(((f->flags&SF_SHARE) && f->extent < 0) ||
				(f->extent >= 0 && (f->here+n) >= f->extent) ||
				(r = f->size) <= (ssize_t)n)
					r = (ssize_t)n;

			if(r != (ssize_t)n) /* fill stream buffer, then copy */
				r = SFRD(f,f->next,r,f->disc);
			else if((r = SFRD(f,s,r,f->disc)) > 0) /* user buffer */
			{	s += r;
				if((n -= r) <= 0)
					break;
			}
			
			if(r == 0) /* must be eof */
				break;
		}
	}

	SFOPEN(f,local);
	return s-begs;
}
