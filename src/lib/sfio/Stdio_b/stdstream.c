#include	"sfstdio.h"

/*	Translation between sfio and stdio
**	Written by Kiem-Phong Vo
*/

/* symbol to force loading of this file */
int	_Stdio_load = 1;


/* synchronizing sfio and stdio buffer pointers */
#if __STD_C
static int _sfstdsync(Sfio_t* sf)
#else
static int _sfstdsync(sf)
Sfio_t*	sf;
#endif
{
	FILE*	f;

	if(!(f = (FILE*)sf->stdio) )
		return -1;

	SFMTXLOCK(sf);

	if(MUSTSYNC(f))
	{
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
		CLRSYNC(f);
	}

	SFMTXUNLOCK(sf);

	return 0;
}


/* initializing the mapping between standard streams */
#define STDINIT()	(sfstdin->stdio ? 0 : _stdinit())
static int _stdinit()
{
	vtmtxlock(_Sfmutex);

	if(!sfstdin->stdio)
	{	_Sfstdsync = _sfstdsync;

		sfstdin->stdio = stdin;
		sfstdout->stdio = stdout;
		sfstdout->stdio = stderr;
#if _has_std_sf
		stdin->std_sf = sfstdin;
		stdout->std_sf = sfstdout;
		stderr->std_sf = sfstderr;
#endif
	}

	vtmtxunlock(_Sfmutex);

	return 0;
}


/* map a FILE stream to a Sfio_t stream */
#if __STD_C
Sfio_t* _sfstream(reg FILE* f)
#else
Sfio_t* _sfstream(f)
reg FILE*	f;
#endif
{
	reg Sfio_t*	sf;
	reg Sfpool_t*	p;
	reg int		n;

	STDINIT();

	sf = NIL(Sfio_t*);
	if(f)
	{
#if _has_std_sf
		sf = f->std_sf;
#endif
		vtmtxlock(_Sfmutex);
		if(!sf)	/* slow search for sf */
		{	if(f == stdin)
				sf = sfstdin;
			else if(f == stdout)
				sf = sfstdout;
			else if(f == stderr)
				sf = sfstderr;
			else
			{	for(p = &_Sfpool; p && !sf; p = p->next)
					for(n = p->n_sf-1; n >= 0 && !sf; --n)
						if(p->sf[n]->stdio == (Void_t*)f)
							sf = p->sf[n];
			}
		}
		vtmtxunlock(_Sfmutex);
	}

	if(sf)	/* synchronize data pointers */
	{	_sfstdsync(sf);
		if(!OKSF(sf))
		{	int	rv, flags;

			flags = sf->flags; sf->flags |= SF_SHARE|SF_PUBLIC;
			rv = _sfmode(sf,0,0);
			sf->flags = flags;

			if(rv < 0)
				sfclrlock(sf);
		}
		_stdclrerr(f);
	}

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

	STDINIT();

	if(!sf)
		return NIL(FILE*);

	if(!(f = sf->stdio) && !(f = (FILE*)malloc(sizeof(FILE))) )
		return NIL(FILE*);
	memclear(f,sizeof(FILE));

	SFMTXLOCK(sf);

#if _FILE_flag || _FILE_flags
	f->std_flag =	((sf->flags&SF_EOF) ? _IOEOF : 0) |
			((sf->flags&SF_ERROR) ? _IOERR : 0);
#endif
#if _FILE_file || _FILE_fileno
	f->std_file = sffileno(sf);
#endif

	/* mapping between corresponding streams */
	sf->stdio = (Void_t*)f;
#if _has_std_sf
	f->std_sf = sf;
#endif

	SFMTXUNLOCK(sf);

	return f;
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

	FORCE(flockfile);
	FORCE(ftrylockfile);
	FORCE(funlockfile);

	FORCE(snprintf);
	FORCE(vsnprintf);
#if _lib___snprintf
	FORCE(__snprintf);
#endif
#if _lib___vsnprintf
	FORCE(__vsnprintf);
#endif
}
