#include	"sfstdio.h"

/*	Translation between sfio and stdio
**	Written by Kiem-Phong Vo
*/

/* symbol to force loading of this file */
int	_Stdio_load = 1;

/* mapping between sfio and stdio */
typedef struct _sfstdmap_s	Sfstdmap_t;
struct _sfstdmap_s
{	FILE*		f;
	Sfio_t*		sf;
	Sfstdmap_t*	next;
};
static Sfstdmap_t*	Map = NIL(Sfstdmap_t*);

/* synchronizing sfio and stdio buffer pointers */
#if __STD_C
int _sfstdio(reg Sfio_t* sf)
#else
int _sfstdio(sf)
reg Sfio_t*	sf;
#endif
{
	FILE*	f;

	if(!(f = _stdstream(sf)))
		return -1;

#if _FILE_readptr
	if(f->std_readptr >= sf->next && f->std_readptr <= sf->endb)
		sf->next = f->std_readptr;
	f->std_readptr = f->std_readend = NIL(uchar*);
#endif
#if _FILE_writeptr
	if(f->std_writeptr >= sf->next && f->std_writeptr <= sf->endb)
		sf->next = f->std_writeptr;
	f->std_writeptr = f->std_writeend = NIL(uchar*);
#endif
#if _FILE_cnt || _FILE_r || _FILE_w
	if(f->std_ptr >= sf->next && f->std_ptr <= sf->endb)
		sf->next = f->std_ptr;
	f->std_ptr = NIL(uchar*);
#endif
#if _FILE_cnt
	f->std_cnt = 0;
#endif
#if _FILE_r
	f->std_r = 0;
#endif
#if _FILE_w
	f->std_w = 0;
#endif

#if _FILE_readptr || _FILE_writeptr || _FILE_cnt || _FILE_w || _FILE_r
	if((sf->mode &= ~SF_STDIO) == SF_READ)
		sf->endr = sf->endb;
	else if(sf->mode == SF_WRITE && !(sf->flags&SF_LINE) )
		sf->endw = sf->endb;
#endif

	return 0;
}


/* map a FILE stream to a Sfio_t stream */
#if __STD_C
Sfio_t* _sfstream(reg FILE* f)
#else
Sfio_t* _sfstream(f)
reg FILE*	f;
#endif
{	reg Sfstdmap_t*	m;
	reg Sfstdmap_t*	p;
	reg Sfio_t*	sf;

	if(!f)
		return NIL(Sfio_t*);

	sf = NIL(Sfio_t*);
	if(f == stdin)
		sf = sfstdin;
	else if(f == stdout)
		sf = sfstdout;
	else if(f == stderr)
		sf = sfstderr;
	else for(p = NIL(Sfstdmap_t*), m = Map; m; p = m, m = m->next)
	{	if(m->f != f)
			continue;

		/* found */
		if(p)
		{	/* move-to-front heuristic */
			p->next = m->next;
			m->next = Map;
			Map = m;
		}
		sf = m->sf;
		break;
	}

	/* synchronize the data pointers */
	if(sf)
		_sfstdio(sf);

	return sf;
}

/* map a Sfio_t stream to a FILE stream */
#if __STD_C
FILE* _stdstream(reg Sfio_t* sf)
#else
FILE* _stdstream(sf)
reg Sfio_t*	sf;
#endif
{	reg FILE*	f;
	reg Sfstdmap_t*	m;
	reg Sfstdmap_t*	p;

	if(!sf)
		return NIL(FILE*);
	else if(sf == sfstdin)
		return stdin;
	else if(sf == sfstdout)
		return stdout;
	else if(sf == sfstderr)
		return stderr;

	for(p = NIL(Sfstdmap_t*), m = Map; m; p = m, m = m->next)
	{	if(m->sf != sf)
			continue;

		if(p) /* move-to-front heuristic */
		{	p->next = m->next;
			m->next = Map;
			Map = m;
		}
		return m->f;
	}

	/* create a new one */
	if(!(f = (FILE*)malloc(sizeof(FILE))) )
		return NIL(FILE*);
	memclear(f,sizeof(FILE));

	if(!(m = (Sfstdmap_t*)malloc(sizeof(Sfstdmap_t))) )
	{	free((Void_t*)f);
		return NIL(FILE*);
	}
	memclear(m,sizeof(Sfstdmap_t));

#if _FILE_flag || _FILE_flags
	f->std_flag =	((sf->flags&SF_EOF) ? _IOEOF : 0) |
			((sf->flags&SF_ERROR) ? _IOERR : 0);
#endif
#if _FILE_file || _FILE_fileno
	f->std_file = sffileno(sf);
#endif

	m->f = f;
	m->sf = sf;
	m->next = Map;
	Map = m;

	return f;
}

/* close the map structure of a FILE stream */
#if __STD_C
void _stdclose(FILE* f)
#else
void _stdclose(f)
reg FILE*	f;
#endif
{	reg Sfstdmap_t*	m;
	reg Sfstdmap_t*	p;

	for(p = NIL(Sfstdmap_t*), m = Map; m; p = m, m = m->next)
	{	if(m->f != f)
			continue;

		if(p)
			p->next = m->next;
		else	Map = m->next;

		free((Void_t*)f);
		free((Void_t*)m);

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
#if _do_uflow
	FORCE(_uflow);
#endif
#if _do_filbuf
	FORCE(_filbuf);
#endif
#if _do_srget
	FORCE(_srget);
#endif
#if _do_sgetc
	FORCE(_sgetc);
#endif

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
	FORCE(_cleanup);
#if _do_overflow
	FORCE(_overflow);
#endif
#if _do_flsbuf
	FORCE(_flsbuf);
#endif
#if _do_swbuf
	FORCE(_swbuf);
#endif
#if _do_sputc
	FORCE(_sputc);
#endif

	FORCE(fseek);
	FORCE(ftell);
	FORCE(rewind);
	FORCE(fflush);
	FORCE(fpurge);
	FORCE(fsetpos);
	FORCE(fgetpos);
}
