#include	"sfdchdr.h"
#include	<errno.h>
#include	<fcntl.h>

/*	Discipline to invoke UNIX processes to filter data.
**	These processes must be able to fit in pipelines.
**
**	Sfdisc_t* sfdcnewfilter(char* program):
**		Create a discipline that runs "program" to process data
**		from a stream before letting the application see such data.
**	int sfdcdelfilter(Sfdisc_t* disc):
**		Delete the discipline.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 03/06/92.
*/

typedef struct _filter_
{
	Sfdisc_t	disc;		/* discipline structure */
	Sfio_t*		filter;		/* the filter stream */
	char		raw[1024];	/* raw data buffer */
	char*		next;		/* remainder of data unwritten to pipe */
	char*		endb;		/* end of data */
} Filter_t;

/* read data from the filter */
#if __STD_C
static ssize_t filterread(Sfio_t* f, Void_t* buf, size_t n, Sfdisc_t* disc)
#else
static ssize_t filterread(f, buf, n, disc)
Sfio_t*		f;	/* stream reading from */
Void_t*		buf;	/* buffer to read into */
size_t		n;	/* number of bytes requested */
Sfdisc_t*	disc;	/* discipline */
#endif
{
	reg Filter_t*	fdisc;
	ssize_t		r, w;

	fdisc = (Filter_t*)disc;
	for(;;)
	{	if(!fdisc->next)
			fdisc->next = fdisc->endb = fdisc->raw;
		else
		{	/* try to get data from filter, if any */
			errno = 0;
			if((r = sfread(fdisc->filter,buf,n)) > 0)
				return r;
			if(errno != EWOULDBLOCK)
				return 0;
		}

		/* get some raw data to stuff down the pipe */
		if(fdisc->next >= fdisc->endb)
		{	if((r = sfrd(f,fdisc->raw,sizeof(fdisc->raw),disc)) > 0)
			{	fdisc->next = fdisc->raw;
				fdisc->endb = fdisc->raw+r;
			}
			else
			{	/* eof, close write end of pipes */
				sfset(fdisc->filter,SF_READ,0);
				close(sffileno(fdisc->filter));
				sfset(fdisc->filter,SF_READ,1);
			}
		}

		if((w = fdisc->endb - fdisc->next) > 0)
		{	errno = 0;
			if((w = sfwrite(fdisc->filter,fdisc->next,w)) > 0)
				fdisc->next += w;
			else if(errno != EWOULDBLOCK)
				return 0;
			/* pipe is full, sleep for a while, then continue */
			else	sleep(1);
		}
	}
}

/* for the duration of this discipline, the stream is unseekable */
#if __STD_C
static Sfoff_t filterseek(Sfio_t* f, Sfoff_t addr, int offset, Sfdisc_t* disc)
#else
static Sfoff_t filterseek(f, addr, offset, disc)
Sfio_t*		f;
Sfoff_t		addr;
int		offset;
Sfdisc_t*	disc;
#endif
{	f = NIL(Sfio_t*);
	addr = 0;
	offset = 0;
	disc = NIL(Sfdisc_t*);
	return (Sfoff_t)(-1);
}

/* on close, remove the discipline */
#if __STD_C
static filterexcept(Sfio_t* f, int type, Void_t* data, Sfdisc_t* disc)
#else
static filterexcept(f,type,data,disc)
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
		else	sfdcdelfilter(disc);
	}

	return 0;
}

#if __STD_C
Sfdisc_t* sfdcnewfilter(char* program)
#else
Sfdisc_t* sfdcnewfilter(program)
char*	program;	/* program to run as a filter */
#endif
{
	reg Filter_t*	disc;
	reg Sfio_t*	filter;

	/* open filter for read&write */
	if(!(filter = sfpopen(NIL(Sfio_t*),program,"r+")) )
		return NIL(Sfdisc_t*);

	/* unbuffered so that write data will get to the pipe right away */
	sfsetbuf(filter,NIL(Void_t*),0);

	/* make the write descriptor nonblocking */
	sfset(filter,SF_READ,0);
	fcntl(sffileno(filter),F_SETFL,FNDELAY);
	sfset(filter,SF_READ,1);

	/* same for the read descriptor */
	sfset(filter,SF_WRITE,0);
	fcntl(sffileno(filter),F_SETFL,FNDELAY);
	sfset(filter,SF_WRITE,1);

	if(!(disc = (Filter_t*)malloc(sizeof(Filter_t))) )
	{	sfclose(filter);
		return NIL(Sfdisc_t*);
	}

	disc->disc.readf = filterread;
	disc->disc.seekf = filterseek;
	disc->disc.writef = NIL(Sfwrite_f);
	disc->disc.exceptf = filterexcept;
	disc->filter = filter;
	disc->next = disc->endb = NIL(char*);

	return (Sfdisc_t*)disc;
}


#if __STD_C
sfdcdelfilter(Sfdisc_t* disc)
#else
sfdcdelfilter(disc)
Sfdisc_t*	disc;
#endif
{
	sfclose(((Filter_t*)disc)->filter);
	free(disc);
	return 0;
}


#ifdef PROGRAM
/*	The below program uncompresses a compressed file,
**	then print all the lines contain the word "define"
**	in upper case.
*/
main()
{
	Sfdisc_t*	uncompress;
	Sfdisc_t*	upper;
	Sfdisc_t*	grep;

	/* insert the uncompress discipline first */
	if(!(uncompress = sfdcnewfilter("uncompress")) )
		return -1;
	sfdisc(sfstdin,uncompress);

	/* now insert the grep discipline */
	if(!(grep = sfdcnewfilter("grep define")) )
		return -1;
	sfdisc(sfstdin,grep);

	/* now insert the lower->upper case discipline */
	if(!(upper = sfdcnewfilter("tr a-z A-Z")) )
		return -1;
	sfdisc(sfstdin,upper);

	/* now just copy data from stdin to stdout */
	sfmove(sfstdin,sfstdout,-1,-1);

	return 0;
}

#endif /*PROGRAM*/
