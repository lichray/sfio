#include	"sfstdio.h"

/*	Translation between sfio and stdio
**	Written by Kiem-Phong Vo
*/

/* symbol to force loading of this file */
int	_Stdio_load = 1;

/* lists of active and free FILE streams */
static FILE	*Files;
static FILE	*Ffree;

/* fast FILE allocation functions */
#define FALLOC(f)	((f = Ffree) ? (Ffree = (FILE*)f->std_next, f) : \
					(f = (FILE*) malloc(sizeof(FILE))))
#define FFREE(f)	(f->std_next = (uchar*)Ffree, Ffree = f)


/* synchronizing sfio and stdio buffer pointers */
#if __STD_C
int _sfstdio(reg Sfio_t* sp)
#else
int _sfstdio(sp)
reg Sfio_t	*sp;
#endif
{
	FILE	*fp;

	if(!(fp = _stdstream(sp)))
		return -1;
#if _FILE_cnt
	if(fp->std_ptr >= sp->next && fp->std_ptr <= sp->endb)
		sp->next = fp->std_ptr;
	fp->std_cnt = 0;
	fp->std_ptr = NIL(uchar*);

	if((sp->mode &= ~SF_STDIO) == SF_READ)
		sp->endr = sp->endb;
	else if(sp->mode == SF_WRITE && !(sp->flags&SF_LINE) )
		sp->endw = sp->endb;
#endif

	return 0;
}


/* map a FILE stream to a Sfio_t stream */
#if __STD_C
Sfio_t* _sfstream(reg FILE* std)
#else
Sfio_t* _sfstream(std)
reg FILE*	std;
#endif
{	reg FILE	*fp, *last;
	reg Sfio_t	*sp;

	/* not one of ours */
	if(!std
#if _FILE_flag
	   || (std->std_flag & (~(_IOEOF|_IOERR)))
#endif
	  )
		return NIL(Sfio_t*);

	sp = NIL(Sfio_t*);
	if(std == stdin)
		sp = sfstdin;
	else if(std == stdout)
		sp = sfstdout;
	else if(std == stderr)
		sp = sfstderr;
	else for(last = NIL(FILE*), fp = Files; fp; last = fp, fp = (FILE*)fp->std_next)
	{	if(fp != std)
			continue;

		/* found */
		if(last)
		{	/* move-to-front heuristic */
			last->std_next = fp->std_next;
			fp->std_next = (uchar*)Files;
			Files = fp;
		}
		sp = (Sfio_t*)fp->std_sf;
		break;
	}

	/* synchronize the data pointers */
	if(sp)
		_sfstdio(sp);

	return sp;
}

/* map a Sfio_t stream to a FILE stream */
#if __STD_C
FILE* _stdstream(reg Sfio_t* sf)
#else
FILE* _stdstream(sf)
reg Sfio_t*	sf;
#endif
{	reg FILE	*fp, *last;

	if(!sf)
		return NIL(FILE*);
	else if(sf == sfstdin)
		return stdin;
	else if(sf == sfstdout)
		return stdout;
	else if(sf == sfstderr)
		return  stderr;

	for(last = NIL(FILE*), fp = Files; fp; last = fp, fp = (FILE*)fp->std_next)
	{	if(fp->std_sf != (uchar*)sf)
			continue;

		/* found */
		if(last)
		{	/* move-to-front heuristic */
			last->std_next = fp->std_next;
			fp->std_next = (uchar*)Files;
			Files = fp;
		}
		return fp;
	}

	/* create a new one */
	if(!FALLOC(fp))
		return NIL(FILE*);
	memclear(fp,sizeof(FILE));
#if _FILE_cnt
	fp->std_cnt = 0;
#endif
#if _FILE_flag
	fp->std_flag =	((sf->flags&SF_EOF) ? _IOEOF : 0) |
			((sf->flags&SF_ERROR) ? _IOERR : 0);
#endif
#if _FILE_file
	fp->std_file = sffileno(sf);
#endif
	fp->std_sf = (uchar*)sf;
	fp->std_next = (uchar*)Files;
	Files = fp;

	return fp;
}

/* close the map structure of a FILE stream */
#if __STD_C
void _stdclose(FILE* std)
#else
void _stdclose(std)
reg FILE	*std;
#endif
{	reg FILE	*fp, *last;

	for(last = NIL(FILE*), fp = Files; fp; last = fp, fp = (FILE*)fp->std_next)
	{	if(fp != std)
			continue;
		if(last)
			last->std_next = fp->std_next;
		else	Files = (FILE*)fp->std_next;
		FFREE(fp);
		return;
	}
}


/* The following functions are never called from the application code.
   They are here to force loading of all compatibility functions and
   avoid name conflicts that arise from certain local implementations.
*/
#define FORCE(f)	((void)__stdforce((Void_t*)f) )

#if __STD_C
static Void_t* __stdforce(Void_t* fun)
#else
static Void_t* __stdforce(fun)
Void_t* fun;
#endif
{
	return(fun);
}

void __stdiold()
{	
	FORCE(clearerr);
	FORCE(feof);
	FORCE(ferror);
	FORCE(fileno);
	FORCE(setbuf);
	FORCE(setbuffer);
	FORCE(setlinebuf);
	FORCE(setvbuf);

	FORCE(fclose);
	FORCE(fdopen);
	FORCE(fopen);
	FORCE(freopen);
	FORCE(popen);
	FORCE(pclose);
	FORCE(tmpfile);

	FORCE(fgetc);
	FORCE(getc);
	FORCE(getchar);
	FORCE(fgets);
	FORCE(gets);
	FORCE(getw);
	FORCE(fread);
	FORCE(fscanf);
	FORCE(scanf);
	FORCE(sscanf);
	FORCE(_doscan);
	FORCE(vfscanf);
	FORCE(vscanf);
	FORCE(vsscanf);
	FORCE(ungetc);
	FORCE(_filbuf);

	FORCE(fputc);
	FORCE(putc);
	FORCE(putchar);
	FORCE(fputs);
	FORCE(puts);
	FORCE(putw);
	FORCE(fwrite);
	FORCE(fprintf);
	FORCE(printf);
	FORCE(sprintf);
	FORCE(_doprnt);
	FORCE(vfprintf);
	FORCE(vprintf);
	FORCE(vsprintf);
	FORCE(_flsbuf);
	FORCE(_cleanup);

	FORCE(fseek);
	FORCE(ftell);
	FORCE(rewind);
	FORCE(fflush);
	FORCE(fpurge);
	FORCE(fsetpos);
	FORCE(fgetpos);
}
