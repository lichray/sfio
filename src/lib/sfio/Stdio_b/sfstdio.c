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

#if __STD_C
#if __cplusplus
extern "C" {
#endif
extern int	printf(const char*,...);
extern char*	strcmp(const char*,const char*);
#if __cplusplus
}
#endif
#endif

/* standard names for fields of interest */
#include	"sfstdhdr.h"
#include	"FEATURE/sfio"

#if _FILE_cnt
#define	std_cnt		_cnt
#endif

#if _FILE_ptr
#define std_ptr		_ptr
#endif

#if _FILE_flag
#define std_flag	_flag
#endif

#if _FILE_file
#define std_file	_file
#endif

#if _FILE_base
#define std_sf		_base
#endif

#if _FILE_bufsiz
#define std_next	_bufsiz
#else
#if _FILE_bufsize
#define std_next	_bufsize
#endif
#endif

/* required fields in the FILE structure */
#define STD_CNT		1
#define STD_PTR		2
#define STD_FLAG	3
#define STD_FILE	4
#define STD_SF		5
#define STD_NEXT	6

typedef struct _fld_
{	int		offset;
	int		size;
	int		type;
	struct _fld_	*next;
} Field_t;

static FILE	_F_;
#define uchar			unsigned char
#define Offsetof(member)	(((uchar*)(&(_F_.member))) - ((uchar*)(&_F_)))
#define Sizeof(member)		sizeof(_F_.member)
#define NIL(type)		((type)0)

#if __STD_C
Field_t *fldinsert(Field_t* head, Field_t* f)
#else
Field_t *fldinsert(head,f)
Field_t	*head, *f;
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
char	*name;
char	*type;
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
	Field_t	ffile, fcnt, fptr, fflag, fsf, fnext, *list, *f;
	int	pos, gap, s, n_init;
	char	*inits[16];

	/* construct the order list of the fields of interest */
	list = NIL(Field_t*);

#ifdef std_cnt
	fcnt.offset = Offsetof(std_cnt);
	fcnt.size = Sizeof(std_cnt);
	fcnt.type = STD_CNT;
	fcnt.next = NIL(Field_t*);
	list = fldinsert(list,&fcnt);
#endif

#ifdef std_ptr
	fptr.offset = Offsetof(std_ptr);
	fptr.size = Sizeof(std_ptr);
	fptr.type = STD_PTR;
	fptr.next = NIL(Field_t*);
	list = fldinsert(list,&fptr);
#endif

#ifdef std_flag
	fflag.offset = Offsetof(std_flag);
	fflag.size = Sizeof(std_flag);
	fflag.type = STD_FLAG;
	fflag.next = NIL(Field_t*);
	list = fldinsert(list,&fflag);
#endif

#ifdef std_file
	ffile.offset = Offsetof(std_file);
	ffile.size = Sizeof(std_file);
	ffile.type = STD_FILE;
	ffile.next = NIL(Field_t*);
	list = fldinsert(list,&ffile);
#endif

#ifdef std_sf
	fsf.offset = Offsetof(std_sf);
	fsf.size = Sizeof(std_sf);
	fsf.type = STD_SF;
	fsf.next = NIL(Field_t*);
	list = fldinsert(list,&fsf);
#endif

#ifdef std_next
	fnext.offset = Offsetof(std_next);
	fnext.size = Sizeof(std_next);
	fnext.type = STD_NEXT;
	fnext.next = NIL(Field_t*);
	list = fldinsert(list,&fnext);
#endif

	/* output standard protection */
	printf("#ifndef	_STDIO_INIT\n\n");

	printf("extern int\t_Stdio_load;\n\n");

	if(strcmp(NAME_iob,"_iob") != 0)
		printf("#define _iob	%s\n\n",NAME_iob);
	if(strcmp(NAME_filbuf,"_filbuf") != 0)
		printf("#define _filbuf	%s\n\n",NAME_filbuf);
	if(strcmp(NAME_flsbuf,"_flsbuf") != 0)
		printf("#define _flsbuf	%s\n\n",NAME_flsbuf);

#if _pragma_weak
	printf("#if __STD_C\n");
	printf("#pragma weak _doprnt =	__doprnt\n");
	printf("#pragma weak _doscan =	__doscan\n");
	printf("#pragma weak %s =	_%s\n",NAME_filbuf,NAME_filbuf);
	printf("#pragma weak %s =	_%s\n",NAME_flsbuf,NAME_flsbuf);
	printf("#pragma weak %s =	_%s\n",NAME_iob,NAME_iob);

	printf("#define _doprnt	__doprnt\n");
	printf("#define _doscan	__doscan\n");
	printf("#define %s	_%s\n",NAME_filbuf,NAME_filbuf);
	printf("#define %s	_%s\n",NAME_flsbuf,NAME_flsbuf);
	printf("#define %s	_%s\n",NAME_iob,NAME_iob);
	printf("#endif\n\n");
#endif

#ifdef _NFILE
	printf("#define _NFILE	%d\n\n", _NFILE);
#else
	printf("#define _NFILE	%d\n\n", 3);
#endif

	/* now get what we need from sfio */
	printf("#include\t\"sfhdr.h\"\n\n");

	/* output FILE structure */
	printf("typedef struct _std_\tFILE;\n");
	printf("struct _std_\n{\n");
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
		case STD_SF :
			fldprint(f->size,"std_sf","uchar*");
			inits[n_init++] = "(uchar*)0";
			break;
		case STD_NEXT :
			fldprint(f->size,"std_next","uchar*");
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

#ifndef std_sf
#define std_endif
	printf("#ifndef _STDEXTERN\n");
	printf("\tuchar*\tstd_sf;\n");
#endif
#ifndef std_next
#ifndef std_endif
#define std_endif
	printf("#ifndef _STDEXTERN\n");
#endif
	printf("\tuchar*\tstd_next;\n");
#endif
#ifdef std_endif
	printf("#endif\n");
#endif

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
#define _IOERR	0
#endif
#ifndef _IOEOF
#define _IOEOF	0
#endif
#ifndef _IONBF
#define _IONBF	0
#endif
#ifndef _IOLBF
#define _IOLBF	0
#endif
#ifndef _IOFBF
#define _IOFBF	0
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

	printf("#define stdfile(n)\t((FILE*)(((char*)(&_iob[0]))+(n)*%d))\n",
		sizeof(FILE));
	printf("#define stdin\t\tstdfile(0)\n");
	printf("#define stdout\t\tstdfile(1)\n");
	printf("#define stderr\t\tstdfile(2)\n\n");

	printf("#if __cplusplus\n");
	printf("extern \"C\" {\n");
	printf("#endif\n");

	printf("extern FILE\t\t_iob[];\n");

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
	printf("extern int\t\tfileno _ARG_((FILE*));\n");
	printf("extern int\t\t_flsbuf _ARG_((int, FILE*));\n");
	printf("extern void\t\t_cleanup _ARG_((void));\n");
	printf("extern FILE*\t\tfopen _ARG_((char*, const char*));\n");
	printf("extern int\t\tfprintf _ARG_((FILE*, const char* , ...));\n");
	printf("extern int\t\tfputc _ARG_((int, FILE*));\n");
	printf("extern int\t\tfputs _ARG_((const char*, FILE*));\n");
	printf("extern int\t\tfread _ARG_((void*, int, int, FILE*));\n");
	printf("extern FILE*\t\tfreopen _ARG_((char*, const char*, FILE*));\n");
	printf("extern int\t\tfscanf _ARG_((FILE*, const char* , ...));\n");
	printf("extern int\t\tfseek _ARG_((FILE*, long , int));\n");
	printf("extern long\t\tftell _ARG_((FILE*));\n");
	printf("extern int\t\tfgetpos _ARG_((FILE*,long*));\n");
	printf("extern int\t\tfsetpos _ARG_((FILE*,long*));\n");
	printf("extern int\t\tfpurge _ARG_((FILE*));\n");
	printf("extern int\t\tfwrite _ARG_((const void*, int, int, FILE*));\n");
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
	printf("extern int\t\tsetbuffer _ARG_((FILE*, char*, int));\n");
	printf("extern int\t\tsetlinebuf _ARG_((FILE*));\n");
	printf("extern int\t\tsetvbuf _ARG_((FILE*,char*,int,int));\n");
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
