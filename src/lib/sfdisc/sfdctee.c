#include	"sfdchdr.h"

/*	A discipline to tee the output to a stream to another stream.
**	This is similar to what the "tee" program does. As implemented
**	this discipline only works with file streams.
**
**	Sfdisc_t* sfdcnewtee(Sfio_t* tee):
**		Create a discipline that would tee data to a stream to
**		the stream "tee".
**	int sfdcdeltee(Sfdisc_t* teedisc):
**		Delete the discipline.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 03/05/92.
*/

/* the discipline structure for tee-ing */
typedef struct _tee_
{
	Sfdisc_t	disc;	/* the sfio discipline structure */
	Sfio_t*		tee;	/* the stream to tee to */
	int		status;	/* if tee stream is still ok */
} Tee_t;

/*	write to the teed stream.  */
#if __STD_C
static ssize_t teewrite(Sfio_t* f, const Void_t* buf, size_t size, Sfdisc_t* disc)
#else
static ssize_t teewrite(f,buf,size,disc)
Sfio_t* 	f;	/* the stream being written to */
Void_t*		buf;	/* the buffer of data being output */
size_t		size;	/* the data size */
Sfdisc_t*	disc;	/* the tee discipline */
#endif
{
	reg Tee_t*	tdisc = (Tee_t*)disc;

	/* tee data if still ok */
	if(tdisc->status == 0 && sfwrite(tdisc->tee,buf,size) != size)
		tdisc->status = -1;

	/* do the actual write */
	return sfwr(f,buf,size,disc);
}

/* on close, remove the discipline */
#if __STD_C
static teeexcept(Sfio_t* f, int type, Void_t* data, Sfdisc_t* disc)
#else
static teeexcept(f,type,data,disc)
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
		else	sfdcdeltee(disc);
	}

	return 0;
}

#if __STD_C
Sfdisc_t* sfdcnewtee(Sfio_t* tee)
#else
Sfdisc_t* sfdcnewtee(tee)
Sfio_t*	tee;	/* stream to tee to */
#endif
{
	reg Tee_t*	disc;

	if(!(disc = (Tee_t*)malloc(sizeof(Tee_t))) )
		return NIL(Sfdisc_t*);

	disc->disc.readf = NIL(Sfread_f);
	disc->disc.seekf = NIL(Sfseek_f);
	disc->disc.writef = teewrite;
	disc->disc.exceptf = teeexcept;
	disc->tee = tee;
	disc->status = 0;

	return (Sfdisc_t*)disc;
}

#if __STD_C
sfdcdeltee(Sfdisc_t* disc)
#else
sfdcdeltee(disc)
Sfdisc_t*	disc;	/* tee discipline being ended */
#endif
{
	free(disc);
	return 0;
}

#ifdef PROGRAM
/*	Here is the UNIX tee program
*/

#if __STD_C
main(int argc, char** argv)
#else
main(argc,argv)
int	argc;
char**	argv;
#endif
{
	Sfdisc_t*	disc;
	Sfio_t*		tee;

	if(argc <= 1)
	{	sfprintf(sfstderr,"Called as: %s file\n", argv[0]);
		return -1;
	}

	/* open the file to tee data to */
	if(!(tee = sfopen(NIL(Sfio_t*),argv[1],"w")) )
	{	sfprintf(sfstderr,"%s: Can't create %s\n", argv[0], argv[1]);
		return -1;
	}

	/* make a tee discipline and insert it */
	if(!(disc = sfdcnewtee(tee)) )
		return -1;
	sfdisc(sfstdout,disc);

	/* now just copy data from stdin to stdout */
	sfmove(sfstdin,sfstdout,-1,-1);

	return 0;
}

#endif /*PROGRAM*/
