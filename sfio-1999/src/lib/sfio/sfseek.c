#include	"sfhdr.h"

/*	Set the IO pointer to a specific location in the stream
**
**	Written by Kiem-Phong Vo (03/24/98)
*/

#if __STD_C
static void newpos(Sfio_t* f, Sfoff_t p)
#else
static void newpos(f, p)
Sfio_t*	f;
Sfoff_t p;
#endif
{
#ifdef MAP_TYPE
	if((f->bits&SF_MMAP) && f->data)
	{	SFMUNMAP(f, f->data, f->endb-f->data);
		f->data = NIL(uchar*);
	}
#endif
	f->next = f->endr = f->endw = f->data;
	f->endb = (f->mode&SF_WRITE) ? f->data+f->size : f->data;
	if((f->here = p) < 0)
	{	f->extent = -1;
		f->here = 0;
	}
}

#if __STD_C
Sfoff_t sfseek(Sfio_t* f, Sfoff_t p, int type)
#else
Sfoff_t sfseek(f,p,type)
Sfio_t*	f;	/* seek to a new location in this stream */
Sfoff_t	p;	/* place to seek to */
int	type;	/* 0: from org, 1: from here, 2: from end */
#endif
{
	Sfoff_t	r, s, d;
	size_t	a;
	reg int	mode, local, hardseek, mustsync;

	GETLOCAL(f,local);

	hardseek = (type|f->flags)&(SF_SHARE|SF_PUBLIC);

	if(hardseek && f->mode == (SF_READ|SF_SYNCED) )
	{	newpos(f,f->here);
		f->mode = SF_READ;
	}

	/* set and initialize the stream to a definite mode */
	if((int)SFMODE(f,local) != (mode = f->mode&SF_RDWR) && _sfmode(f,mode,local) < 0 )
		return -1;

	mustsync = (type&SF_SHARE) && !(type&SF_PUBLIC) &&
		   (f->mode&SF_READ) && !(f->flags&SF_STRING);

	if((type &= (0|1|2)) < 0 || type > 2 || f->extent < 0)
		return -1;

	/* throw away ungetc data */
	if(f->disc == _Sfudisc)
		(void)sfclose((*_Sfstack)(f,NIL(Sfio_t*)));

	/* lock the stream for internal manipulations */
	SFLOCK(f,local);

	/* clear error and eof bits */
	f->flags &= ~(SF_EOF|SF_ERROR);

	while(f->flags&SF_STRING)
	{	SFSTRSIZE(f);

		if(type == 1)
			r = p + (f->next - f->data);
		else if(type == 2)
			r = p + f->extent;
		else	r = p;

		if(r >= 0 && r <= f->size)
		{	p = r;
			f->next = f->data+p;
			f->here = p;
			if(p > f->extent)
				memclear((char*)(f->data+f->extent),(int)(p-f->extent));
			goto done;
		}

		/* check exception handler, note that this may pop stream */
		if(SFSK(f,r,0,f->disc) != r)
		{	p = -1;
			goto done;
		}
		else if(!(f->flags&SF_STRING))
		{	p = r;
			goto done;
		}
	}

	if(f->mode&SF_WRITE)
	{	/* see if we can avoid flushing buffer */
		if(!hardseek && type < 2 && !(f->flags&SF_APPENDWR) )
		{	s = f->here + (f->next - f->data);
			r = p + (type == 0 ? 0 : s);
			if(s == r)
			{	p = r;
				goto done;
			}
		}

		if(f->next > f->data && SFSYNC(f) < 0)
		{	p = -1;
			goto done;
		}
	}

	if(type == 2 || (f->mode&SF_WRITE) )
	{	if((hardseek&SF_PUBLIC) || type == 2)
			p = SFSK(f, p, type, f->disc);
		else
		{	r = p + (type == 1 ? f->here : 0);
			p = (hardseek || r != f->here) ? SFSK(f, r, 0, f->disc) : r;
		}
		if(p >= 0)
			newpos(f,p);
		goto done;
	}

	/* must be a read stream */
	s = f->here - (f->endb - f->next);
	r = p + (type == 1 ? s : 0);
	if(r <= f->here && r >= (f->here - (f->endb-f->data)) )
	{	if((hardseek || (type == 1 && p == 0)) )
		{	if((s = SFSK(f, (Sfoff_t)0, 1, f->disc)) == f->here ||
			   (s >= 0 && !(hardseek&SF_PUBLIC) &&
			    (s = SFSK(f, f->here, 0, f->disc)) == f->here) )
				goto near_done;
			else if(s < 0)
			{	p = -1;
				goto done;
			}
			else
			{	newpos(f,s);
				hardseek = 0;
			}
		}
		else
		{ near_done:
			f->next = f->endb - (f->here - r);
			p = r;
			goto done;
		}
	}

	/* desired position */
	if((p += type == 1 ? s : 0) < 0)
		goto done;

#ifdef MAP_TYPE
	if(f->bits&SF_MMAP)
	{	/* if mmap is not great, stop mmaping if moving around too much */
#if _mmap_worthy < 2
		if((f->next - f->data) < ((f->endb - f->data)/4) )
		{	SFSETBUF(f,(Void_t*)f->tiny,(size_t)SF_UNBOUND);
			hardseek = 1; /* this forces a hard seek below */
		}
		else
#endif
		{	/* for mmap, f->here can be virtual */
			newpos(f,p);
			goto done;
		}
	}
#endif

	/* buffer is now considered empty */
	f->next = f->endr = f->endb = f->data;

	/* small backseeks often come in bunches, so align with buffer */
	if(f->size <= SF_PAGE || (p < s && (s-p) <= f->size) )
		a = f->size;
	/* otherwise, align with page size */
	else	a = SF_PAGE;

	/* d is amount of buffered space that must remain after 'p' is aligned */
	if((d = s-p) <= 0 || d >= (Sfoff_t)f->size) /* forward seek or long backseek */
		d = f->size/2;
	if(d > SF_GRAIN)
		d = SF_GRAIN;

	/* align only if we believe there is enough buffered data to read */
	if(p >= f->extent || (f->flags&SF_RDWR) == SF_RDWR ||
	   (p - (r = (p/a)*a)) >= ((Sfoff_t)f->size)-d )
		r = p;

	if((hardseek || r != f->here) && (f->here = SFSK(f,r,0,f->disc)) != r)
	{	if(r != p) /* the rounded seek failed, now try to just get to p */
			f->here = SFSK(f,p,0,f->disc);
		if(f->here != p)
			p = -1;
		goto done;
	}

	if(r < p) /* read to cover p */
	{	if((f->size-a) < SF_GRAIN)
			a = f->size;
		(void)SFRD(f, f->data, a, f->disc);
		if(p <= f->here && p >= (f->here - (f->endb - f->data)) )
			f->next = f->endb - (int)(f->here-p);
		else /* recover from read failure by just seeking to p */
		{	f->next = f->endb = f->data;
			if((f->here = SFSK(f,p,0,f->disc)) != p)
				p = -1;
		}
	}

done :
	if(f->here < 0) /* hasn't been the best of time */
	{	f->extent = -1;
		f->here = 0;
	}

	if(p >= 0)
		f->bits |= SF_JUSTSEEK;

	SFOPEN(f,local);

	if(mustsync)
		sfsync(f);
	return p;
}
