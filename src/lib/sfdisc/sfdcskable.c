#include	"sfdchdr.h"

/*	Discipline to make an unseekable read stream seekable
**
**	Written by (Kiem-)Phong Vo, kpv@research.att.com, 08/18/92.
*/

typedef struct _skable_
{
	Sfdisc_t	disc;	/* sfio discipline */
	Sfio_t*		shadow;	/* to shadow data */
	int		eof;	/* if eof has been reached */
} Seek_t;

#ifdef __STD_C
static skwrite(Sfio_t* f, char* buf, int n, Sfdisc_t* disc)
#else
static skwrite(f, buf, n, disc)
Sfio_t*	f;	/* stream involved */
char*		buf;	/* buffer to read into */
int		n;	/* number of bytes to read */
Sfdisc_t*	disc;	/* discipline */
#endif
{
	return -1;
}

#ifdef __STD_C
static skread(Sfio_t* f, char* buf, int n, Sfdisc_t* disc)
#else
static skread(f, buf, n, disc)
Sfio_t*	f;	/* stream involved */
char*		buf;	/* buffer to read into */
int		n;	/* number of bytes to read */
Sfdisc_t*	disc;	/* discipline */
#endif
{
	Seek_t*		sk;
	Sfio_t*	sf;
	long		addr, extent;
	int		r, w;

	sk = (Seek_t*)disc;
	sf = sk->shadow;
	if(sk->eof)
		return sfread(sf,buf,n);

	addr = sfseek(sf,0L,1);
	extent = sfsize(sf);

	if(addr+n <= extent)
		return sfread(sf,buf,n);

	if((r = (int)(extent-addr)) > 0)
	{	if((w = sfread(sf,buf,r)) != r)
			return w;
		buf += r;
		n -= r;
	}
		
	/* do a raw read */
	if((n = sfrd(f,buf,n,disc)) <= 0)
	{	sk->eof = 1;
		n = 0;
	}
	else if(sfwrite(sf,buf,n) != n)
		sk->eof = 1;

	return r+n;
}

#ifdef __STD_C
static long skseek(Sfio_t* f, long addr, int type, Sfdisc_t* disc)
#else
static long skseek(f, addr, type, disc)
Sfio_t*	f;
long		addr;
int		type;
Sfdisc_t*	disc;
#endif
{
	long		extent;
	Seek_t*		sk;
	Sfio_t*	sf;
	char		buf[SF_BUFSIZE];
	int		r, w;

	if(type < 0 || type > 2)
		return -1L;

	sk = (Seek_t*)disc;
	sf = sk->shadow;

	extent = sfseek(sf,0L,2);
	if(type == 1)
		addr += sftell(sf);
	else if(type == 2)
		addr += extent;

	if(addr < 0)
		return -1L;
	else if(addr > extent)
	{	if(sk->eof)
			return -1L;

		/* read enough to reach the seek point */
		while(addr > extent)
		{	if(addr > extent+sizeof(buf) )
				w = sizeof(buf);
			else	w = (int)(addr-extent);
			if((r = sfrd(f,buf,w,disc)) <= 0)
				w = r-1;
			else if((w = sfwrite(sf,buf,r)) > 0)
				extent += r;
			if(w != r)
			{	sk->eof = 1;
				break;
			}
		}

		if(addr > extent)
			return -1L;
	}

	return sfseek(sf,addr,0);
}

/* on close, remove the discipline */
#ifdef __STD_C
static skexcept(Sfio_t* f, int type, Sfdisc_t* disc)
#else
static skexcept(f,type,disc)
Sfio_t*	f;
int		type;
Sfdisc_t*	disc;
#endif
{
	if(type == SF_CLOSE)
	{	reg Sfdisc_t*	pop;
		if((pop = sfdisc(f,SF_POPDISC)) != disc)	/* hmm! */
			sfdisc(f,pop);
		else	sfdcdelskable(disc);
	}

	return 0;
}

#ifdef __STD_C
Sfdisc_t* sfdcnewskable(Sfio_t* f)
#else
Sfdisc_t* sfdcnewskable(f)
Sfio_t*	f;
#endif
{
	reg Seek_t*	disc;

	if(!(disc = (Seek_t*)malloc(sizeof(Seek_t))) )
		return NIL(Sfdisc_t*);

	if(sfseek(f,0L,1) >= 0)
	{	/* if already seekable, do nothing */
		disc->disc.readf = NIL(int(*)_ARG_((Sfio_t*,char*,int,Sfdisc_t*)) );
		disc->disc.writef = NIL(int(*)_ARG_((Sfio_t*,char*,int,Sfdisc_t*)) );
		disc->disc.seekf = NIL(long(*)_ARG_((Sfio_t*,long,int,Sfdisc_t*)) );
		disc->disc.exceptf = NIL(int(*)_ARG_((Sfio_t*,int,Sfdisc_t*)) );
		disc->eof = 0;
	}
	else
	{	disc->disc.readf = skread;
		disc->disc.writef = skwrite;
		disc->disc.seekf = skseek;
		disc->disc.exceptf = skexcept;
		disc->shadow = sftmp(SF_BUFSIZE);
		disc->eof = 0;
	}

	return (Sfdisc_t*)disc;
}

#ifdef __STD_C
sfdcdelskable(Sfdisc_t* disc)
#else
sfdcdelskable(disc)
Sfdisc_t*	disc;
#endif
{
	sfclose(((Seek_t*)disc)->shadow);
	free((char*)disc);
	return 0;
}


#ifdef PROGRAM
/*	Seek forward some number of bytes, seek back, read and print
*/

main()
{
	Sfdisc_t*	disc;

	sfprintf(sfstdout,"Type three lines of 1234, abcd, efgh then ctrl-D\n");
	if(!(disc = sfdcnewskable(sfstdin)) )
		return -1;
	sfdisc(sfstdin,disc);
	sfseek(sfstdin,15L,0);
	sfseek(sfstdin,-10L,1);
	sfmove(sfstdin,sfstdout,5L,-1);
	sfprintf(sfstdout,"Last line should be: abcd\n");

	return 0;
}

#endif /*PROGRAM*/
