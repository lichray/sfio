#include	"sfdchdr.h"


/*	Concatenate a sequence of streams to a single stream.
**	This is for reading only.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 08/27/92.
*/

#define	UNSEEKABLE	1

typedef struct _f_
{
	Sfio_t*	f;	/* the stream		*/
	Sfoff_t	lower;	/* its lowest end	*/
} File_t;

typedef struct _union_
{
	Sfdisc_t	disc;	/* discipline structure */
	short		type;	/* type of streams	*/
	short		c;	/* current stream	*/
	short		n;	/* number of streams	*/
	Sfoff_t		here;	/* current location	*/
	File_t		f[1];	/* array of streams	*/
} Union_t;

#if __STD_C
static ssize_t unwrite(Sfio_t* f, const Void_t* buf, size_t n, Sfdisc_t* disc)
#else
static ssize_t unwrite(f, buf, n, disc)
Sfio_t*        f;      /* stream involved */
Void_t*        buf;    /* buffer to read into */
size_t         n;      /* number of bytes to read */
Sfdisc_t*      disc;   /* discipline */
#endif
{
	return -1;
}

#if __STD_C
static ssize_t unread(Sfio_t* f, Void_t* buf, size_t n, Sfdisc_t* disc)
#else
static ssize_t unread(f, buf, n, disc)
Sfio_t*        f;      /* stream involved */
Void_t*        buf;    /* buffer to read into */
size_t         n;      /* number of bytes to read */
Sfdisc_t*      disc;   /* discipline */
#endif
{
	reg Union_t*	un;
	reg ssize_t	r, m;

	un = (Union_t*)disc;
	m = n;
	f = un->f[un->c].f;
	while(1)
	{	if((r = sfread(f,buf,m)) < 0 || (r == 0 && un->c == un->n-1) )
			break;

		m -= r;
		un->here += r;

		if(m == 0)
			break;

		buf = (char*)buf + r;
		if(sfeof(f) && un->c < un->n-1)
			f = un->f[un->c += 1].f;
	}
	return n-m;
}

#if __STD_C
static Sfoff_t unseek(Sfio_t* f, Sfoff_t addr, int type, Sfdisc_t* disc)
#else
static Sfoff_t unseek(f, addr, type, disc)
Sfio_t*        f;
Sfoff_t        addr;
int            type;
Sfdisc_t*      disc;
#endif
{
	reg Union_t*	un;
	reg int		i;
	reg Sfoff_t	extent, s;

	un = (Union_t*)disc;
	if(un->type&UNSEEKABLE)
		return -1L;

	if(type == 2)
	{	extent = 0;
		for(i = 0; i < un->n; ++i)
			extent += (sfsize(un->f[i].f) - un->f[i].lower);
		addr += extent;
	}
	else if(type == 1)
		addr += un->here;

	if(addr < 0)
		return -1;

	/* find the stream where the addr could be in */
	extent = 0;
	for(i = 0; i < un->n-1; ++i)
	{	s = sfsize(un->f[i].f) - un->f[i].lower;
		if(addr < extent + s)
			break;
		extent += s;
	}

	s = (addr-extent) + un->f[i].lower;
	if(sfseek(un->f[i].f,s,0) != s)
		return -1;

	un->c = i;
	un->here = addr;

	for(i += 1; i < un->n; ++i)
		sfseek(un->f[i].f,un->f[i].lower,0);

	return addr;
}

/* on close, remove the discipline */
#if __STD_C
static unexcept(Sfio_t* f, int type, Void_t* data, Sfdisc_t* disc)
#else
static unexcept(f,type,data,disc)
Sfio_t*		f;
int		type;
Void_t*		data;
Sfdisc_t*	disc;
#endif
{
	if(type == SF_CLOSE)
	{	reg Sfdisc_t*	pop;
		if((pop = sfdisc(f,SF_POPDISC)) != disc)	/* hmm! */
			sfdisc(f,pop);
		else	sfdcdelunion(disc);
	}

	return 0;
}

#if __STD_C
Sfdisc_t* sfdcnewunion(Sfio_t** f, int n)
#else
Sfdisc_t* sfdcnewunion(f, n)
Sfio_t**	f;
int		n;
#endif
{
	reg Union_t*	un;
	reg int		i;

	if(n <= 0)
		return NIL(Sfdisc_t*);

	if(!(un = (Union_t*)malloc(sizeof(Union_t)+(n-1)*sizeof(File_t))) )
		return NIL(Sfdisc_t*);

	un->disc.readf = unread;
	un->disc.writef = unwrite;
	un->disc.seekf = unseek;
	un->disc.exceptf = unexcept;
	un->type = 0;
	un->c = 0;
	un->n = n;
	un->here = 0;
	for(i = 0; i < n; ++i)
	{
		un->f[i].f = f[i];
		if(!(un->type&UNSEEKABLE))
		{	un->f[i].lower = sfseek(f[i],0L,1);
			if(un->f[i].lower < 0)
				un->type |= UNSEEKABLE;
		}
	}

	return (Sfdisc_t*)un;
}

#if __STD_C
sfdcdelunion(Sfdisc_t* disc)
#else
sfdcdelunion(disc)
Sfdisc_t*	disc;
#endif
{
	free(disc);
	return 0;
}

#ifdef PROGRAM
main()
{
	Sfio_t	*s[2];
	Sfdisc_t	*disc;

	if(!(s[0] = sfopen(NIL(Sfio_t*),"abcdefghij", "s")) ||
	   !(s[1] = sfopen(NIL(Sfio_t*),"0123456789", "s")) )
		return -1;

	sfseek(s[0],5,0);
	sfseek(s[1],5,0);

	if(!(disc = sfdcnewunion(s,2)) )
		return -1;

	sfdisc(sfstdin,disc);
	sfmove(sfstdin,sfstdout,-1,-1);
	sfputc(sfstdout,'\n');

	sfseek(sfstdin,7,0);
	sfmove(sfstdin,sfstdout,-1,-1);
	sfputc(sfstdout,'\n');
}
#endif /*PROGRAM*/
