#include	"sfdchdr.h"


/*	Discipline to treat a contiguous subset of a stream as a stream
**	in its own right. The hard part in all this is to allow multiple
**	segments of the stream to be used as subfiles at the same time.
**
**	Sfdisc_t* sfdcnewsubstream(Sfio_t* f, long offset, long extent):
**		Create a new discipline that would make some other stream
**		a substream of the stream "f" starting at location "offset"
**		and having size "extent". If "extent" is <0, the substream
**		covers everything starting from "offset".
**	sfdcdelsubstream(Sfdisc_t* disc):
**		Delete a substream.
**
**	Written by David G. Korn and (Kiem-)Phong Vo (03/06/92)
*/

typedef struct _subfile_
{
	Sfdisc_t	disc;	/* sfio discipline */
	Sfio_t*	parent;	/* parent stream */
	long		offset;	/* starting offset */
	long		extent;	/* size wanted */
	long		here;	/* current seek location */
} Subfile_t;

#ifdef __STD_C
static streamio(Sfio_t* f, char* buf, int n, Sfdisc_t* disc, int type)
#else
static streamio(f, buf, n, disc, type)
Sfio_t*	f;
char*		buf;
int		n;
Sfdisc_t*	disc;
int		type;
#endif
{
	reg Subfile_t	*sdisc;
	reg long	here, parent;
	reg int		io;

	sdisc = (Subfile_t*)disc;

	/* read just what we need */
	if(sdisc->extent >= 0 && n > (io = (int)(sdisc->extent - sdisc->here)) )
		n = io;
	if(n <= 0)
		return n;

	/* save current location in parent stream */
	parent = sfseek(sdisc->parent,0L,1);

	/* read data */
	here = sdisc->here + sdisc->offset;
	if(sfseek(sdisc->parent,here,0) != here)
		io = 0;
	else
	{	if(type == SF_WRITE) 
			io = sfwrite(sdisc->parent,buf,n);
		else	io = sfread(sdisc->parent,buf,n);
		if(io > 0)
			sdisc->here += io;
	}

	/* restore parent current position */
	sfseek(sdisc->parent,parent,0);

	return io;
}

#ifdef __STD_C
static streamwrite(Sfio_t* f, char* buf, int n, Sfdisc_t* disc)
#else
static streamwrite(f, buf, n, disc)
Sfio_t*	f;
char*		buf;
int		n;
Sfdisc_t*	disc;
#endif
{
	return streamio(f,buf,n,disc,SF_WRITE);
}

#ifdef __STD_C
static streamread(Sfio_t* f, char* buf, int n, Sfdisc_t* disc)
#else
static streamread(f, buf, n, disc)
Sfio_t*	f;
char*		buf;
int		n;
Sfdisc_t*	disc;
#endif
{
	return streamio(f,buf,n,disc,SF_READ);
}

#ifdef __STD_C
static long streamseek(Sfio_t* f, long pos, int type, Sfdisc_t* disc)
#else
static long streamseek(f, pos, type, disc)
Sfio_t*	f;
long		pos;
int		type;
Sfdisc_t*	disc;
#endif
{
	reg Subfile_t*	sdisc;
	reg long	here, parent;

	sdisc = (Subfile_t*)disc;

	switch(type)
	{
	case 0:
		here = 0L;
		break;
	case 1:
		here = sdisc->here;
		break;
	case 2:
		if(sdisc->extent >= 0)
			here = sdisc->extent;
		else
		{	parent = sfseek(sdisc->parent,0L,1);
			if((here = sfseek(sdisc->parent,0L,2)) < 0)
				return -1L;
			else	here -= sdisc->offset;
			sfseek(sdisc->parent,parent,0L);
		}
		break;
	default:
		return -1L;
	}

	pos += here;
	if(pos < 0 || (sdisc->extent >= 0 && pos >= sdisc->extent))
		return -1L;

	return (sdisc->here = pos);
}

#ifdef __STD_C
static streamexcept(Sfio_t* f, int type, Sfdisc_t* disc)
#else
static streamexcept(f, type, disc)
Sfio_t*	f;
int		type;
Sfdisc_t*	disc;
#endif
{
	if(type == SF_CLOSE)
	{	reg Sfdisc_t*	pop;
		if((pop = sfdisc(f,SF_POPDISC)) != disc)	/* hmm! */
			sfdisc(f,pop);
		else	sfdcdelsubstream(disc);
	}

	return 0;
}

#ifdef __STD_C
Sfdisc_t* sfdcnewsubstream(Sfio_t* f, long offset, long extent)
#else
Sfdisc_t* sfdcnewsubstream(f, offset, extent)
Sfio_t*	f;	/* stream */
long		offset;	/* offset in f */
long		extent;	/* desired size */
#endif
{
	reg Subfile_t*	sdisc;
	reg long	here;

	/* establish that we can seek to offset */
	if((here = sfseek(f,0L,1)) < 0 || sfseek(f,offset,0) < 0)
		return NIL(Sfdisc_t*);
	else	sfseek(f,here,0);

	if(!(sdisc = (Subfile_t*)malloc(sizeof(Subfile_t))) )
		return NIL(Sfdisc_t*);

	sdisc->disc.readf = streamread;
	sdisc->disc.writef = streamwrite;
	sdisc->disc.seekf = streamseek;
	sdisc->disc.exceptf = streamexcept;
	sdisc->parent = f;
	sdisc->offset = offset;
	sdisc->extent = extent;

	return (Sfdisc_t*)sdisc;
}


#ifdef __STD_C
sfdcdelsubstream(Sfdisc_t* disc)
#else
sfdcdelsubstream(disc)
Sfdisc_t*	disc;
#endif
{
	free((char*)disc);
	return 0;
}


#ifdef PROGRAM
/*	The following program mixes lines from the top half and
**	bottom half of a file. It only works if stdin is seekable.
*/
main()
{
	Sfio_t		*top, *bot;
	Sfdisc_t	*topdisc, *botdisc;
	char		*s1, *s2;
	long		size;

	/* find midpoint */
	if((size = sfsize(sfstdin)) < 0 || sfseek(sfstdin,(size /= 2)-1,0) < 0)
		return -1;

	/* skip to next line */
	while(sfgetc(sfstdin) != '\n')
		size += 1;

	/* create dummy streams for top and bottom halves of the file */
	if(!(top = sfnew(NIL(Sfio_t*),NIL(char*),-1,1001,SF_READ)) ||
	   !(bot = sfnew(NIL(Sfio_t*),NIL(char*),-1,1001,SF_READ)) )
		return -1;

	/* create and install disciplines */
	if(!(topdisc = sfdcnewsubstream(sfstdin,0L,size)) ||
	   !(botdisc = sfdcnewsubstream(sfstdin,size,-1L)) )
		return -1;
	sfdisc(top,topdisc);
	sfdisc(bot,botdisc);

	/* now mix lines */
	for(;;)
	{
		if(s1 = sfgetr(top,'\n',0))
			sfwrite(sfstdout,s1,sfslen());
		if(s2 = sfgetr(bot,'\n',0))
			sfwrite(sfstdout,s2,sfslen());
		if(!s1 && !s2)
			return 0;
	}
}

#endif /* PROGRAM */
