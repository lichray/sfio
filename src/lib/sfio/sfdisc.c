#include	"sfhdr.h"

/*	Set a new discipline for a stream.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#ifdef sfdisc	/* KPV-- to be removed on next release */
#undef sfdisc

typedef int(* Oldexcept_f)_ARG_((Sfio_t*, int, Sfdisc_t*));

typedef struct olddisc_s
{	Sfdisc_t	fake;
	Sfdisc_t*	orig;
} Olddisc_t;

#if __STD_C
static ssize_t fakeread(Sfio_t* f, Void_t* buf, size_t n, Sfdisc_t* disc)
#else
static ssize_t fakeread(f, buf, n, disc)
Sfio_t*		f;
Void_t*		buf;
size_t		n;
Sfdisc_t*	disc;
#endif
{	Olddisc_t*	d = (Olddisc_t*)disc;
	return (*d->orig->readf)(f,buf,n,d->orig);
}
#if __STD_C
static ssize_t fakewrite(Sfio_t* f, const Void_t* buf, size_t n, Sfdisc_t* disc)
#else
static ssize_t fakewrite(f, buf, n, disc)
Sfio_t*		f;
Void_t*		buf;
size_t		n;
Sfdisc_t*	disc;
#endif
{	Olddisc_t*	d = (Olddisc_t*)disc;
	return (*d->orig->writef)(f,buf,n,d->orig);
}
#if __STD_C
static Sfoff_t fakeseek(Sfio_t* f, Sfoff_t pos, int type, Sfdisc_t* disc)
#else
static Sfoff_t fakeseek(f, pos, type, disc)
Sfio_t*		f;
Sfoff_t		pos;
int		type;
Sfdisc_t*	disc;
#endif
{	Olddisc_t*	d = (Olddisc_t*)disc;
	return (*d->orig->seekf)(f,pos,type,d->orig);
}
#if __STD_C
static int fakeexcept(Sfio_t* f, int type, Void_t* value, Sfdisc_t* disc)
#else
static int fakeexcept(f, type, value, disc)
Sfio_t*		f;
int		type;
Void_t*		value;
Sfdisc_t*	disc;
#endif
{	Olddisc_t*	d = (Olddisc_t*)disc;
	Oldexcept_f	exceptf = (Oldexcept_f)d->orig->exceptf;
	int		rv;

	if((rv = (*exceptf)(f,type,d->orig)) == 0 &&
	   (type == SF_CLOSE || type == SF_DPOP) )
		free((Void_t*)d);

	return rv;
}

#if __STD_C
Sfdisc_t* sfdisc(Sfio_t* f, Sfdisc_t* disc)
#else
Sfdisc_t* sfdisc(f, disc)
Sfio_t*		f;
Sfdisc_t*	disc;
#endif
{	Olddisc_t*	d;

	if(!disc)
		return _sfdisc(f,NIL(Sfdisc_t*));

	d = (Olddisc_t*)malloc(sizeof(Olddisc_t));
	d->fake.readf = disc->readf ? fakeread : NIL(Sfread_f);
	d->fake.writef = disc->writef ? fakewrite : NIL(Sfwrite_f);
	d->fake.seekf = disc->seekf ? fakeseek : NIL(Sfseek_f);
	d->fake.exceptf = disc->exceptf ? fakeexcept : NIL(Sfexcept_f);
	d->orig = disc;
	if(_sfdisc(f, &d->fake) == &d->fake)
		return disc;
	else
	{	free((Void_t*)d);
		return NIL(Sfdisc_t*);
	}
}

#define sfdisc _sfdisc
#endif /*sfdisc*/

#if __STD_C
Sfdisc_t* sfdisc(reg Sfio_t* f, reg Sfdisc_t* disc)
#else
Sfdisc_t* sfdisc(f,disc)
reg Sfio_t*	f;
reg Sfdisc_t*	disc;
#endif
{
	reg Sfdisc_t	*d, *rdisc;
	reg Sfread_f	oreadf;
	reg Sfwrite_f	owritef;
	reg Sfseek_f	oseekf;
	ssize_t		n;

	if((f->flags&SF_READ) && (f->bits&SF_PROCESS) && (f->mode&SF_WRITE) )
	{	/* make sure in read mode to check for read-ahead data */
		if(_sfmode(f,SF_READ,0) < 0)
			return NIL(Sfdisc_t*);
	}
	else if((f->mode&SF_RDWR) != f->mode && _sfmode(f,0,0) < 0)
		return NIL(Sfdisc_t*);

	SFLOCK(f,0);
	rdisc = NIL(Sfdisc_t*);

	/* synchronize before switching to a new discipline */
	if(!(f->flags&SF_STRING))
	{	if(((f->mode&SF_WRITE) && f->next > f->data) ||
		   (f->mode&SF_READ) || f->disc == _Sfudisc )
			(void)SFSYNC(f);

		if(((f->mode&SF_WRITE) && (n = f->next-f->data) > 0) ||
		   ((f->mode&SF_READ) && f->extent < 0 && (n = f->endb-f->next) > 0) )
		{
			reg Sfexcept_f	exceptf;
			reg int		rv = 0;

			exceptf = disc ? disc->exceptf : f->disc ? f->disc->exceptf :
				  NIL(Sfexcept_f);

			/* check with application for course of action */
			if(exceptf)
			{	SFOPEN(f,0);
				rv = (*exceptf)(f, SF_DBUFFER, &n,
						disc ? disc : f->disc);
				SFLOCK(f,0);
			}

			/* can't switch discipline at this time */
			if(rv <= 0)
				goto done;
		}
	}

	/* save old readf, writef, and seekf to see if stream need reinit */
#define GETDISCF(func,iof,type) \
	{ for(d = f->disc; d && !d->iof; d = d->disc) ; \
	  func = d ? d->iof : NIL(type); \
	}
	GETDISCF(oreadf,readf,Sfread_f);
	GETDISCF(owritef,writef,Sfwrite_f);
	GETDISCF(oseekf,seekf,Sfseek_f);

	if(disc == SF_POPDISC)
	{	/* popping, warn the being popped discipline */
		if(!(d = f->disc) )
			goto done;
		disc = d->disc;
		if(d->exceptf)
		{	SFOPEN(f,0);
			if((*(d->exceptf))(f,SF_DPOP,(Void_t*)disc,d) < 0 )
				goto done;
			SFLOCK(f,0);
		}
		f->disc = disc;
		rdisc = d;
	}
	else
	{	/* pushing, warn being pushed discipline */
		do
		{	/* loop to handle the case where d may pop itself */
			d = f->disc;
			if(d && d->exceptf)
			{	SFOPEN(f,0);
				if( (*(d->exceptf))(f,SF_DPUSH,(Void_t*)disc,d) < 0 )
					goto done;
				SFLOCK(f,0);
			}
		} while(d != f->disc);

		/* make sure we are not creating an infinite loop */
		for(; d; d = d->disc)
			if(d == disc)
				goto done;

		/* set new disc */
		disc->disc = f->disc;
		f->disc = disc;
		rdisc = disc;
	}

	if(!(f->flags&SF_STRING) )
	{	/* this stream may have to be reinitialized */
		reg int	reinit = 0;
#define DISCF(dst,iof,type)	(dst ? dst->iof : NIL(type)) 
#define REINIT(oiof,iof,type) \
		if(!reinit) \
		{	for(d = f->disc; d && !d->iof; d = d->disc) ; \
			if(DISCF(d,iof,type) != oiof) \
				reinit = 1; \
		}

		REINIT(oreadf,readf,Sfread_f);
		REINIT(owritef,writef,Sfwrite_f);
		REINIT(oseekf,seekf,Sfseek_f);

		if(reinit)
		{	SETLOCAL(f);
			f->bits &= ~SF_NULL;	/* turn off /dev/null handling */
			if((f->bits&SF_MMAP) || (f->mode&SF_INIT))
				sfsetbuf(f,NIL(Void_t*),(size_t)SF_UNBOUND);
			else if(f->data == f->tiny)
				sfsetbuf(f,NIL(Void_t*),0);
			else
			{	int	flags = f->flags;
				sfsetbuf(f,(Void_t*)f->data,f->size);
				f->flags |= (flags&SF_MALLOC);
			}
		}
	}

done :
	SFOPEN(f,0);
	return rdisc;
}
