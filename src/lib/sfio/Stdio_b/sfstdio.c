/*	Program to generate the header file sfstdio.h used
**	by the stdio-binary-compatible functions in Stdio_b.
**
**	Written by Kiem-Phong Vo.
*/
#if __STD_C
#undef __STD_C
#endif
#if __STDC__
#define	__STD_C		1
#else
#if __cplusplus
#define __STD_C		1
#endif
#endif
#if !defined(__STD_C)
#define __STD_C		0
#endif

#if __STD_C
#if __cplusplus
extern "C" {
#endif
extern int	printf(const char*,...);
extern int	strcmp(const char*,const char*);
#if __cplusplus
}
#endif
#endif

/* standard names for fields of interest */
#include	"sfstdhdr.h"
#include	"FEATURE/sfio"
#include	"FEATURE/stdio"

#if _FILE_cnt
#define	std_cnt		_cnt
#endif

#if _FILE_r
#define std_r		_r
#endif

#if _FILE_w
#define std_w		_w
#endif

#if _FILE_ptr
#define std_ptr		_ptr
#else
#if _FILE_p
#define std_ptr		_p
#endif
#endif

#if _FILE_readptr
#define std_readptr	_IO_read_ptr
#define std_readend	_IO_read_end
#endif

#if _FILE_writeptr
#define std_writeptr	_IO_write_ptr
#define std_writeend	_IO_write_end
#endif

#if _FILE_flag
#define std_flag	_flag
#else
#if _FILE_flags
#define std_flag	_flags
#endif
#endif

#if _FILE_file
#define std_file	_file
#else
#if _FILE_fileno
#define std_file	_fileno
#endif
#endif

/* required fields in the FILE structure */
#define STD_CNT		1
#define STD_PTR		2
#define STD_FLAG	3
#define STD_FILE	4
#define STD_READPTR	5
#define STD_READEND	6
#define STD_WRITEPTR	7
#define STD_WRITEEND	8
#define STD_R		9
#define STD_W		10

typedef struct _fld_
{	int		offset;
	int		size;
	int		type;
	struct _fld_*	next;
} Field_t;

static FILE	_F_;
#define uchar			unsigned char
#define Offsetof(member)	(((uchar*)(&(_F_.member))) - ((uchar*)(&_F_)))
#define Sizeof(member)		sizeof(_F_.member)
#define NIL(type)		((type)0)

#if __STD_C
Field_t* fldinsert(Field_t* head, Field_t* f)
#else
Field_t* fldinsert(head,f)
Field_t*	head;
Field_t*	f;
#endif
{	Field_t *fp, *last;

	if(!head)
		return f;

	for(last = NIL(Field_t*), fp = head; fp; last = fp, fp = fp->next)
		if(f->offset < fp->offset)
			break;
	if(last)
	{	f->next = last->next;
		last->next = f;
	}
	else
	{	f->next = head;
		head = f;
	}

	return head;
}

#if __STD_C
int fldprint(int size, char* name, char* type)
#else
int fldprint(size,name,type)
int	size;
char*	name;
char*	type;
#endif
{
	if(type)
		printf("\t%s\t%s;\n",type,name);
	else if(size == sizeof(char))
		printf("\tchar\t%s;\n",name);
	else if(size == sizeof(short))
		printf("\tshort\t%s;\n",name);
	else if(size == sizeof(int))
		printf("\tint\t%s;\n",name);
	else	printf("\tlong\t%s;\n",name);
	return 0;
}

int main()
{
	int	pos, gap, s, n_init;
	Field_t	*list, *f;
	Field_t	field[16];
	char*	inits[16];

	/* construct the order list of the fields of interest */
	list = NIL(Field_t*);
	f = field;

#ifdef std_cnt	/* old-stdio _cnt */
	f->offset = Offsetof(std_cnt);
	f->size = Sizeof(std_cnt);
	f->type = STD_CNT;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_r	/* BSD-stdio _r (read count) */
	f->offset = Offsetof(std_r);
	f->size = Sizeof(std_r);
	f->type = STD_R;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_w	/* BSD-stdio _w (write count) */
	f->offset = Offsetof(std_w);
	f->size = Sizeof(std_w);
	f->type = STD_W;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_ptr	/* old-stdio _ptr */
	f->offset = Offsetof(std_ptr);
	f->size = Sizeof(std_ptr);
	f->type = STD_PTR;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_p	/* BSD-stdio _p */
	f->offset = Offsetof(std_p);
	f->size = Sizeof(std_p);
	f->type = STD_PTR;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_readptr /* Linux-stdio _IO_read_ptr */
	f->offset = Offsetof(std_readptr);
	f->size = Sizeof(std_readptr);
	f->type = STD_READPTR;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_readend /* Linux-stdio _IO_read_end */
	f->offset = Offsetof(std_readend);
	f->size = Sizeof(std_readend);
	f->type = STD_READEND;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_writeptr /* Linux-stdio _IO_write_ptr */
	f->offset = Offsetof(std_writeptr);
	f->size = Sizeof(std_writeptr);
	f->type = STD_WRITEPTR;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_writeend /* Linux-stdio _IO_write_end */
	f->offset = Offsetof(std_writeend);
	f->size = Sizeof(std_writeend);
	f->type = STD_WRITEEND;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_flag	/* old-stdio _flag */
	f->offset = Offsetof(std_flag);
	f->size = Sizeof(std_flag);
	f->type = STD_FLAG;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

#ifdef std_file /* old/BSD-stdio _file */
	f->offset = Offsetof(std_file);
	f->size = Sizeof(std_file);
	f->type = STD_FILE;
	f->next = NIL(Field_t*);
	list = fldinsert(list,f);
	f += 1;
#endif

	/* output standard protection */
	printf("#ifndef	_STDIO_INIT\n\n");

	printf("extern int\t_Stdio_load;\n\n");

#ifdef NAME_iob
	if(strcmp(NAME_iob,"_iob") != 0)
		printf("#define _iob	%s\n",NAME_iob);
	printf("#define _do_iob	1\n\n");
#endif

#ifdef NAME_sf
	if(strcmp(NAME_sf,"_sf") != 0)
		printf("#define _sf	%s\n",NAME_sf);
	printf("#define _do_sf	1\n\n");
#endif

#ifdef NAME_filbuf
	if(strcmp(NAME_filbuf,"_filbuf") != 0 )
		printf("#define _filbuf	%s\n",NAME_filbuf);
	if(__STD_C)
		printf("#define FILBUF(f)	int _filbuf(FILE* f)\n");
	else	printf("#define FILBUF(f)	int _filbuf(f) FILE* f;\n");
	printf("#define _do_filbuf	1\n\n");
#endif

#ifdef NAME_flsbuf
	if(strcmp(NAME_flsbuf,"_flsbuf") != 0)
		printf("#define _flsbuf	%s\n",NAME_flsbuf);
	if(__STD_C)
		printf("#define FLSBUF(c,f)	int _flsbuf(int c, FILE* f)\n");
	else	printf("#define FLSBUF(c,f)	int _flsbuf(c,f) int c; FILE* f;\n");
	printf("#define _do_flsbuf	1\n\n");
#endif

#ifdef NAME_uflow
	if(strcmp(NAME_uflow,"_uflow") != 0 )
		printf("#define _uflow	%s\n",NAME_uflow);
	if(__STD_C)
		printf("#define FILBUF(f)	int _uflow(FILE* f)\n");
	else	printf("#define FILBUF(f)	int _uflow(f) FILE* f;\n");
	printf("#define _do_uflow      1\n\n");
#endif

#ifdef NAME_overflow
	if(strcmp(NAME_overflow,"_overflow") != 0)
		printf("#define _overflow	%s\n",NAME_overflow);
	if(__STD_C)
		printf("#define FLSBUF(c,f)	int _overflow(FILE* f, int c)\n");
	else	printf("#define FLSBUF(c,f)	int _overflow(f,c) FILE* f; int c;\n");
	printf("#define _do_overflow       1\n\n");
#endif

#ifdef NAME_srget
	if(strcmp(NAME_srget,"_srget") != 0 )
		printf("#define _srget	%s\n",NAME_srget);
	if(__STD_C)
		printf("#define FILBUF(f)	int _srget(FILE* f)\n");
	else	printf("#define FILBUF(f)	int _srget(f) FILE* f;\n");
	printf("#define _do_srget      1\n\n");
#endif

#ifdef NAME_swbuf
	if(strcmp(NAME_swbuf,"_swbuf") != 0)
		printf("#define _swbuf	%s\n",NAME_swbuf);
	if(__STD_C)
		printf("#define FLSBUF(c,f)	int _swbuf(int c, FILE* f)\n");
	else	printf("#define FLSBUF(c,f)	int _swbuf(c,f) int c; FILE* f;\n");
	printf("#define _do_swbuf       1\n\n");
#endif

#ifdef NAME_sgetc
	if(strcmp(NAME_sgetc,"_sgetc") != 0 )
		printf("#define _sgetc	%s\n",NAME_sgetc);
	if(__STD_C)
		printf("#define FILBUF(f)	int _sgetc(FILE* f)\n");
	else	printf("#define FILBUF(f)	int _sgetc(f) FILE* f;\n");
	printf("#define _do_sgetc      1\n\n");
#endif

#ifdef NAME_sputc
	if(strcmp(NAME_sputc,"_sputc") != 0)
		printf("#define _sputc	%s\n",NAME_sputc);
	if(__STD_C)
		printf("#define FLSBUF(c,f)	int _sputc(int c, FILE* f)\n");
	else	printf("#define FLSBUF(c,f)	int _sputc(c,f) int c; FILE* f;\n");
	printf("#define _do_sputc       1\n\n");
#endif

#if _pragma_weak
	printf("#if _in_doprnt\n");
	printf("#pragma weak _doprnt =	__doprnt\n");
	printf("#define _doprnt	__doprnt\n");
	printf("#endif\n\n");

	printf("#if _in_doscan\n");
	printf("#pragma weak _doscan =	__doscan\n");
	printf("#define _doscan	__doscan\n");
	printf("#endif\n\n");

#ifdef NAME_iob
	printf("#if _in_stdextern\n");
	printf("#pragma weak %s =	_%s\n",NAME_iob,NAME_iob);
	printf("#define %s	_%s\n",NAME_iob,NAME_iob);
	printf("#endif\n\n");
#endif

#ifdef NAME_filbuf
	printf("#if _in_filbuf\n");
	printf("#pragma weak %s =	_%s\n",NAME_filbuf,NAME_filbuf);
	printf("#define %s	_%s\n",NAME_filbuf,NAME_filbuf);
	printf("#endif\n\n");
#endif

#ifdef NAME_flsbuf
	printf("#if _in_flsbuf\n");
	printf("#pragma weak %s =	_%s\n",NAME_flsbuf,NAME_flsbuf);
	printf("#define %s	_%s\n",NAME_flsbuf,NAME_flsbuf);
	printf("#endif\n\n");
#endif

#ifdef NAME_uflow
	printf("#if _do_filbuf\n"); /* not a bug! */
	printf("#pragma weak %s =	_%s\n",NAME_uflow,NAME_uflow);
	printf("#define %s	_%s\n",NAME_uflow,NAME_uflow);
	printf("#endif\n\n");
#endif

#ifdef NAME_overflow
	printf("#if _do_flsbuf\n"); /* not a bug! */
	printf("#pragma weak %s =	_%s\n",NAME_overflow,NAME_overflow);
	printf("#define %s	_%s\n",NAME_overflow,NAME_overflow);
	printf("#endif\n\n");
#endif

#ifdef NAME_srget
	printf("#if _do_filbuf\n"); /* not a bug! */
	printf("#pragma weak %s =	_%s\n",NAME_srget,NAME_srget);
	printf("#define %s	_%s\n",NAME_srget,NAME_srget);
	printf("#endif\n\n");
#endif

#ifdef NAME_swbuf
	printf("#if _do_flsbuf\n"); /* not a bug! */
	printf("#pragma weak %s =	_%s\n",NAME_swbuf,NAME_swbuf);
	printf("#define %s	_%s\n",NAME_swbuf,NAME_swbuf);
	printf("#endif\n\n");
#endif

#ifdef NAME_sgetc
	printf("#if _do_filbuf\n"); /* not a bug! */
	printf("#pragma weak %s =	_%s\n",NAME_sgetc,NAME_sgetc);
	printf("#define %s	_%s\n",NAME_sgetc,NAME_sgetc);
	printf("#endif\n\n");
#endif

#ifdef NAME_sputc
	printf("#if _do_flsbuf\n"); /* not a bug! */
	printf("#pragma weak %s =	_%s\n",NAME_sputc,NAME_sputc);
	printf("#define %s	_%s\n",NAME_sputc,NAME_sputc);
	printf("#endif\n\n");
#endif

#endif/*_pragma_weak*/

#ifdef _NFILE
	printf("#define _NFILE	%d\n\n", _NFILE);
#else
	printf("#define _NFILE	%d\n\n", 3);
#endif

	/* now get what we need from sfio */
	printf("typedef struct _std_s\tFILE;\n");
	printf("#include\t\"sfhdr.h\"\n");
	printf("#include\t\"FEATURE/stdio\"\n");
	printf("#undef\tFILE\n");

#ifndef _siz_fpos_t
	printf("#define fpos_t	long\n");
#else
	if(_siz_fpos_t == sizeof(int))
		printf("#define fpos_t	int\n");
	else if(_siz_fpos_t == sizeof(long))
		printf("#define fpos_t	long\n");
#if _typ_long_long
	else if(_siz_fpos_t == sizeof(long long))
		printf("#define fpos_t	long long\n");
#endif
	else	printf("#define fpos_t	long\n");
#endif

	/* output FILE structure */
	printf("\nstruct _std_s\n{\n");
	n_init = gap = pos = 0;
	for(f = list; f; f = f->next)
	{	if((s = f->offset - pos) > 0)
		{	/* fill the gap */
			printf("\tuchar\tstd_gap%d[%d];\n",gap++,s);
			inits[n_init++] = "\"\"";
		}

		switch(f->type)
		{
		case STD_CNT :
			fldprint(f->size,"std_cnt",NIL(char*));
			inits[n_init++] = "0";
			break;
		case STD_R :
			fldprint(f->size,"std_r",NIL(char*));
			inits[n_init++] = "0";
			break;
		case STD_W :
			fldprint(f->size,"std_w",NIL(char*));
			inits[n_init++] = "0";
			break;
		case STD_PTR :
			fldprint(f->size,"std_ptr","uchar*");
			inits[n_init++] = "(uchar*)0";
			break;
		case STD_FLAG :
			fldprint(f->size,"std_flag",NIL(char*));
			inits[n_init++] = "0";
			break;
		case STD_FILE :
			fldprint(f->size,"std_file",NIL(char*));
			inits[n_init++] = "(n)";
			break;
		case STD_READPTR :
			fldprint(f->size,"std_readptr","uchar*");
			inits[n_init++] = "(uchar*)0";
			break;
		case STD_READEND :
			fldprint(f->size,"std_readend","uchar*");
			inits[n_init++] = "(uchar*)0";
			break;
		case STD_WRITEPTR :
			fldprint(f->size,"std_writeptr","uchar*");
			inits[n_init++] = "(uchar*)0";
			break;
		case STD_WRITEEND :
			fldprint(f->size,"std_writeend","uchar*");
			inits[n_init++] = "(uchar*)0";
			break;
		}

		pos = f->size+f->offset;
	}

	if((s = sizeof(FILE) - pos) > 0)
	{	/* fill the gap */
		printf("\tuchar\tstd_gap%d[%d];\n",gap++,s);
		inits[n_init++] = "\"\"";
	}

	printf("};\n\n");

	/* output the init macro */
	printf("#define _STDIO_INIT(n)\t{");
	for(s = 0; s < n_init-1; ++s)
		printf("%s, ",inits[s]);
	printf("%s}\n\n",inits[s]);

	/* default values for a few typical stdio constants */
#ifndef BUFSIZ
#define BUFSIZ	1024
#endif
#ifndef _IOERR
#	ifdef _ERR
#		define _IOERR	_ERR
#	else
#		ifdef _IO_ERR_SEEN
#		define _IOERR	_IO_ERR_SEEN
#		else
#			ifdef __SERR
#			define _IOERR	__SERR
#			else
#			define _IOERR	0
#			endif
#		endif
#	endif
#endif
#ifndef _IOEOF
#	ifdef _EOF
#		define _IOEOF	_EOF
#	else
#		ifdef _IO_EOF_SEEN
#		define _IOEOF	_IO_EOF_SEEN
#		else
#			ifdef __SEOF
#			define _IOEOF	__SEOF
#			else
#			define _IOEOF	0
#			endif
#		endif
#	endif
#endif
#ifndef _IONBF
#	ifdef _UNBUF
#		define _IONBF	_UNBUF
#	else
#		ifdef _IO_UNBUFFERED
#		define _IO_NBF	_IO_UNBUFFERED
#		else
#			ifdef __SNBF
#			define _IONBF	__SNBF
#			else
#			define _IONBF	0
#			endif
#		endif
#	endif
#endif
#ifndef _IOLBF
#	ifdef _LINBUF
#		define _IOLBF	_LINBUF
#	else
#		ifdef _IO_LINE_BUF
#		define _IOLBF	_IO_LINE_BUF
#		else
#			ifdef __SLBF
#			define _IOLBF	__SLBF
#			else
#			define _IOLBF	0
#			endif
#		endif
#	endif
#endif
#ifndef _IOFBF
#	define _IOFBF	0
#endif
	printf("#define BUFSIZ\t\t%d\n",BUFSIZ);
	printf("#define _IOERR\t\t%d\n",_IOERR);
	printf("#define _IOEOF\t\t%d\n",_IOEOF);
	printf("#define _IONBF\t\t%d\n",_IONBF);
	printf("#define _IOLBF\t\t%d\n",_IOLBF);
	printf("#define _IOFBF\t\t%d\n\n",_IOFBF);

#if _FILE_flag
	printf("#define _stdeof(fp)\t((fp)->std_flag |= _IOEOF)\n");
	printf("#define _stderr(fp)\t((fp)->std_flag |= _IOERR)\n");
	printf("#define _stdclrerr(fp,sp)\t(((fp)->std_flag &= ~(_IOEOF|_IOERR)),");
	printf("sfclrerr(sp),sfclrlock(sp))\n\n");
#else
	printf("#define _stdeof(fp)\n");
	printf("#define _stderr(fp)\n");
	printf("#define _stdclrerr(fp,sp)\t(sfclrerr(sp),sfclrlock(sp))\n\n");
#endif

	printf("#if __cplusplus\n");
	printf("extern \"C\" {\n");
	printf("#endif\n");
	printf("#if _BLD_SFIO && defined(__EXPORT__)\n");
	printf("#define extern\t__EXPORT__\n");
	printf("#endif\n");

#if !defined(NAME_iob) && !defined(NAME_sf)
	printf("\nextern FILE	_IO_stdin_, _IO_stdout_, _IO_stderr_;\n");
	printf("#define stdin	(&_IO_stdin_)\n");
	printf("#define stdout	(&_IO_stdout_)\n");
	printf("#define stderr	(&_IO_stderr_)\n\n");
#else

#ifdef NAME_iob
	printf("\nextern FILE	_iob[];\n");
	printf("#define stdfile(n)\t((FILE*)(((char*)(&_iob[0]))+(n)*%d))\n",
		sizeof(FILE));
#else
	printf("\nextern FILE	_sf[];\n");
	printf("#define stdfile(n)\t((FILE*)(((char*)(&_sf[0]))+(n)*%d))\n",
		sizeof(FILE));
#endif
	printf("#define stdin	stdfile(0)\n");
	printf("#define stdout	stdfile(1)\n");
	printf("#define stderr	stdfile(2)\n\n");
#endif /*!NAME_iob && !NAME_sf*/

	printf("extern Sfio_t*\t\t_sfstream _ARG_((FILE*));\n");
	printf("extern FILE*\t\t_stdstream _ARG_((Sfio_t*));\n");
	printf("extern int\t\t_sfstdio _ARG_((Sfio_t*));\n");
	printf("extern char*\t\t_stdgets _ARG_((Sfio_t*,char*,int,int));\n");
	printf("extern void\t\t_stdclose _ARG_((FILE*));\n");
	printf("extern int\t\tclearerr _ARG_((FILE*));\n");
	printf("extern int\t\t_doprnt _ARG_((const char*, va_list, FILE*));\n");
	printf("extern int\t\t_doscan _ARG_((FILE*, const char*, va_list));\n");
	printf("extern int\t\tfclose _ARG_((FILE*));\n");
	printf("extern FILE*\t\tfdopen _ARG_((int, const char*));\n");
	printf("extern int\t\tfeof _ARG_((FILE*));\n");
	printf("extern int\t\tferror _ARG_((FILE*));\n");
	printf("extern int\t\tfflush _ARG_((FILE*));\n");
	printf("extern int\t\tfgetc _ARG_((FILE*));\n");
	printf("extern char*\t\tfgets _ARG_((char*, int, FILE*));\n");
	printf("extern int\t\t_filbuf _ARG_((FILE*));\n");
	printf("extern int\t\t_uflow _ARG_((FILE*));\n");
	printf("extern int\t\t_srget _ARG_((FILE*));\n");
	printf("extern int\t\t_sgetc _ARG_((FILE*));\n");
	printf("extern int\t\tfileno _ARG_((FILE*));\n");
	printf("extern int\t\t_flsbuf _ARG_((int, FILE*));\n");
	printf("extern int\t\t_overflow _ARG_((FILE*,int));\n");
	printf("extern int\t\t_swbuf _ARG_((int,FILE*));\n");
	printf("extern int\t\t_sputc _ARG_((int,FILE*));\n");
	printf("extern void\t\t_cleanup _ARG_((void));\n");
	printf("extern FILE*\t\tfopen _ARG_((char*, const char*));\n");
	printf("extern int\t\tfprintf _ARG_((FILE*, const char* , ...));\n");
	printf("extern int\t\tfputc _ARG_((int, FILE*));\n");
	printf("extern int\t\tfputs _ARG_((const char*, FILE*));\n");
	printf("extern size_t\t\tfread _ARG_((void*, size_t, size_t, FILE*));\n");
	printf("extern FILE*\t\tfreopen _ARG_((char*, const char*, FILE*));\n");
	printf("extern int\t\tfscanf _ARG_((FILE*, const char* , ...));\n");
	printf("extern int\t\tfseek _ARG_((FILE*, long , int));\n");
	printf("extern long\t\tftell _ARG_((FILE*));\n");
	printf("extern int\t\tfgetpos _ARG_((FILE*,fpos_t*));\n");
	printf("extern int\t\tfsetpos _ARG_((FILE*,fpos_t*));\n");
	printf("extern int\t\tfpurge _ARG_((FILE*));\n");
	printf("extern size_t\t\tfwrite _ARG_((const void*, size_t, size_t, FILE*));\n");
	printf("extern int\t\tgetc _ARG_((FILE*));\n");
	printf("extern int\t\tgetchar _ARG_((void));\n");
	printf("extern char*\t\tgets _ARG_((char*));\n");
	printf("extern int\t\tgetw _ARG_((FILE*));\n");
	printf("extern int\t\tpclose _ARG_((FILE*));\n");
	printf("extern FILE*\t\tpopen _ARG_((const char*, const char*));\n");
	printf("extern int\t\tprintf _ARG_((const char* , ...));\n");
	printf("extern int\t\tputc _ARG_((int, FILE*));\n");
	printf("extern int\t\tputchar _ARG_((int));\n");
	printf("extern int\t\tputs _ARG_((const char*));\n");
	printf("extern int\t\tputw _ARG_((int, FILE*));\n");
	printf("extern void\t\trewind _ARG_((FILE*));\n");
	printf("extern int\t\tscanf _ARG_((const char* , ...));\n");
	printf("extern void\t\tsetbuf _ARG_((FILE*, char*));\n");
	printf("extern int\t\tsetbuffer _ARG_((FILE*, char*, size_t));\n");
	printf("extern int\t\tsetlinebuf _ARG_((FILE*));\n");
	printf("extern int\t\tsetvbuf _ARG_((FILE*,char*,int,size_t));\n");
	printf("extern int\t\tsprintf _ARG_((char*, const char* , ...));\n");
	printf("extern int\t\tsscanf _ARG_((const char*, const char* , ...));\n");
	printf("extern FILE*\t\ttmpfile _ARG_((void));\n");
	printf("extern int\t\tungetc _ARG_((int,FILE*));\n");
	printf("extern int\t\tvfprintf _ARG_((FILE*, const char* , va_list));\n");
	printf("extern int\t\tvfscanf _ARG_((FILE*, const char* , va_list));\n");
	printf("extern int\t\tvprintf _ARG_((const char* , va_list));\n");
	printf("extern int\t\tvscanf _ARG_((const char* , va_list));\n");
	printf("extern int\t\tvsprintf _ARG_((char*, const char* , va_list));\n");
	printf("extern int\t\tvsscanf _ARG_((char*, const char* , va_list));\n");
	printf("#if __cplusplus\n");
	printf("}\n");
	printf("#endif\n\n");

	printf("#endif /* _STDIO_INIT */\n");

	return 0;
}
