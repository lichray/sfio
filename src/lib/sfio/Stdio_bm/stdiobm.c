#include	<signal.h>
#include	<sys/types.h>
#include	"../ast_common.h"

#define WRLARGE		1
#define RDLARGE		1
#define SKLARGE		1
#define WRSMALL		1
#define RDSMALL		1
#define SKSMALL		1
#define COPY		1
#define SFMOVE		1
#define PUTC		1
#define GETC		1
#define PUTS		1
#define GETS		1
#define SFGETR		1
#define REVGETS		1
#define PRINTF		1
#define SCANF		1

#define LARGE		(8*1024)	/* size for large block IO */
#define SMALL		(64)		/* size for small block IO */

#define SSIZE	2000	/* default # of seek operations	*/
#define BSIZE	20000	/* default # of block I/Os	*/
#define CSIZE	10000	/* default # of getc/putc	*/
#define LSIZE	100000	/* default # of lines		*/

#ifdef PCKORN
#ifdef MSNCC
char	Tmpone[] = "f:\\kpv\\kpvoneXXX";
char	Tmptwo[] = "f:\\kpv\\kpvtwoXXX";
#else
char	Tmpone[] = "/f/kpv/kpvoneXXX";
char	Tmptwo[] = "/f/kpv/kpvtwoXXX";
#endif/*MSNCC*/
#else
char	Tmpone[] = "/tmp/kpvoneXXX";
char	Tmptwo[] = "/tmp/kpvtwoXXX";
#endif

#ifdef MSNCC
struct tms
{	long tms_utime;
	long tms_stime;
	long tms_cutime;
	long tms_cstime;
};
extern long times (struct tms*);
#else
#include	<sys/times.h>
#endif

#ifdef PCKORN
#ifdef MYTIMES
#define TIC_SEC	1000
#endif
#endif

#ifndef TIC_SEC
#define TIC_SEC	60
#endif

_BEGIN_EXTERNS_
extern void	exit _ARG_((int));
extern int	unlink _ARG_((char*));
extern int	atoi _ARG_((char*));
extern char*	strcpy _ARG_((char*, const char*));
_END_EXTERNS_

/* for benchmarking sfio, we want the emulation functions to be as closed
   to their sfio version as possible.
*/
#define _STDCLEAR(f)	(0)
#include	<stdio.h>
#if _SFIO_H
#undef fseek
#undef fread
#undef fwrite
#define fseek(f,p,o)	(sfseek(f, (Sfoff_t)p, o) )
#define fread(d,e,n,f)	(sfread(f, d, e*n)/e)
#define fwrite(d,e,n,f)	(sfwrite(f, d, e*n)/e)
#endif

/*	Program to benchmark/exercise various primitives in stdio.
**	Called as:
**	stdiobm [-b#ofKbytes] [-c#ofKbytes] [-l#ofLines] -s[#ofSeeks]
**		-b: This option affects the tests fwrite/fread etc.
**		    It specifies the number of Kbytes to do io. For example,
**		    -b1024 will do i/o of a megabyte.
**		-c: This option affects the tests putc() and getc().
**		    It specifies the number of Kbytes to do io. For example,
**		    -c1024 will do i/o of a megabyte.
**		-l: This option defines # of times to call  gets(), puts().
**		    printf,scanf do 1/4 of this.
**		-s: This option affects the test for fseek()+fread()+fwrite().
**		    It specifies the number of times to do this combination.
**
**	Written by Kiem-Phong Vo (12/11/90)
*/

#define reg	register

/* quit for any exception */
#if __STD_C
static void error(char* f, int d)
#else
static void error(f,d)
char	*f;
int	d;
#endif
{	fprintf(stderr,f,d);
	exit(-1);
}

/* clean up when killed by a signal such as interrupts */
#if __STD_C
static void sigproc(int sig)
#else
static void sigproc(sig)
int	sig;
#endif
{	
	unlink(Tmpone);
	unlink(Tmptwo);
	error("Quit on signal %d\n",sig);
}

/* print a report */
#if __STD_C
static void report(char* func, int n, int bytes, long real,
		   struct tms* btm, struct tms* etm, char type)
#else
static void report(func,n,bytes,real,btm,etm,type)
char		*func;
int		n, bytes;
long		real;
struct tms	*btm, *etm;
char		type;
#endif
{	
	int		thruput;
	reg long	user, sys;
	static int	first = 1;

	if(first)
	{	first = 0;
		fprintf(stdout,"func\treal\tuser\tsys\tu+s\tsize\tKbytes\tKbytes/s\n");
	}

	user = etm->tms_utime - btm->tms_utime;
	sys  = etm->tms_stime - btm->tms_stime;

	if((user+sys) <= 0)
		thruput = -1;
	else	thruput = (int)(((bytes/1024.)*TIC_SEC)/(user+sys));
	fprintf(stdout,"%s\t%.2f\t%.2f\t%.2f\t%.2f\t%d%c\t%d\t%d\n",
		func,
		real/((float)TIC_SEC),
		user/((float)TIC_SEC),
		sys/((float)TIC_SEC),
		(user+sys)/((float)TIC_SEC),
		n,
		type,
		bytes/1024,
		thruput);
	fflush(stdout);
}

#if __STD_C
unsigned int random(unsigned int seed)
#else
unsigned int random(seed)
unsigned int	seed;
#endif
{
	static unsigned int	current;

	if(seed > 0)
		current = seed;

	return (current = current*0xdeadbeef + 987654321);
}

#if __STD_C
main(int argc, char** argv)
#else
main(argc,argv)
int	argc;
char	**argv;
#endif
{
	reg FILE	*fp, *fw;
	reg int		i, rv, len, v;
	char		data[LARGE];
	struct tms	btm, etm, begtm, endtm;
	long		brtm, ertm, begrtm, endrtm;
	char		*line;
	int		ssize = SSIZE, bsize = BSIZE, csize = CSIZE, lsize = LSIZE;

	while(argc > 1 && argv[1][0] == '-')
	{	switch(argv[1][1])
		{
		case 'b' :
			if((bsize = atoi(argv[1]+2)) < 0)
				bsize = BSIZE;
			break;
		case 'c' :
			if((csize = atoi(argv[1]+2)) < 0)
				csize = CSIZE;
			break;
		case 'l' :
			if((lsize = atoi(argv[1]+2)) < 0)
				lsize = LSIZE;
			break;
		case 's' :
			if((ssize = atoi(argv[1]+2)) < 0)
				ssize = SSIZE;
			break;
		}
		argc--;
		argv++;
	}

	/* do this many megs of byte pushing */
	bsize *= 1024;
	csize *= 1024;

#ifndef MSNCC
	/* quit on the following signals */
        signal(SIGINT,sigproc);
        signal(SIGHUP,sigproc);
        signal(SIGQUIT,sigproc);
#endif
	for(i = 0; i < sizeof(data); ++i)
	{	if((i%16) == 15)
			data[i] = '\n';
		else	data[i] = 'a' + (i%16);
	}

	begrtm = times(&begtm);

	/* construct file names */
#ifndef DEBUG
	if((rv = (int)((unsigned int)begrtm)&0xffff) <= 0)
		rv = 11111;
	for(i = strlen(Tmpone)-1, v = rv; Tmpone[i] == 'X'; --i, v /= 10)
		Tmpone[i] = v%10 + '0';
	for(i = strlen(Tmptwo)-1, v = rv; Tmptwo[i] == 'X'; --i, v /= 10)
		Tmptwo[i] = v%10 + '0';
#endif

#if WRLARGE /* large block writes */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"wb")) )
	{	fprintf(stderr,"Error opening fwrite file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0; i < bsize; )
	{	
		if((rv = fwrite(data, 1, LARGE, fp)) <= 0)
		{	fprintf(stderr,"Error fwrite: %d\n",rv);
			break;
		}
		else	i += rv;
	}
	fclose(fp);

	ertm = times(&etm);

	report("wrlarge",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if RDLARGE /* large block reads */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening fread file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0;;)
	{	if((rv = fread(data, 1, LARGE, fp)) <= 0)
		{	if(rv < 0)
				fprintf(stderr,"Error fread: %d\n",rv);
			break;
		}
		else	i += rv;
	}
	fclose(fp);

	ertm = times(&etm);

	report("rdlarge",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if SKLARGE /* mixing seek/read/write */
	brtm = times(&btm);
	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening seek/read file: %s\n", Tmpone);
		exit(1);
	}
	if(!(fw = fopen(Tmptwo,"wb")) )
	{	fprintf(stderr,"Error opening seek/write file: %s\n", Tmptwo);
		exit(1);
	}

	random(1001);
	for(i = 0, rv = 0; i < ssize; ++i)
	{	reg long	p;
		size_t		r;

		if((p = random(0)%bsize) < 0)
			p = 0;

		if(fseek(fp,p,0) < 0)
			break;
		if((r = fread(data, 1, LARGE, fp)) > 0)
		{	rv += r;

			if(fseek(fw,p,0) < 0)
				break;
			if((r = fwrite(data, 1, r, fw)) > 0)
				rv += r;
		}
	}
	fclose(fp);
	fclose(fw);
	ertm = times(&etm);

	report("sklarge",i,rv,ertm-brtm,&btm,&etm,'S');
#endif

#if WRSMALL /* small block writes */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"wb")) )
	{	fprintf(stderr,"Error opening fwrite file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0; i < bsize; )
	{	
		if((rv = fwrite(data, 1, SMALL, fp)) <= 0)
		{	fprintf(stderr,"Error fwrite: %d\n",rv);
			break;
		}
		else	i += rv;
	}
	fclose(fp);

	ertm = times(&etm);

	report("wrsmall",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if RDSMALL /* small block reads */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening fread file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0;;)
	{	if((rv = fread(data, 1, SMALL, fp)) <= 0)
		{	if(rv < 0)
				fprintf(stderr,"Error fread: %d\n",rv);
			break;
		}
		else	i += rv;
	}
	fclose(fp);

	ertm = times(&etm);

	report("rdsmall",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if SKSMALL /* mixing seek/read random amounts */
	brtm = times(&btm);
	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening seek/read file: %s\n", Tmpone);
		exit(1);
	}
	if(!(fw = fopen(Tmptwo,"wb")) )
	{	fprintf(stderr,"Error opening seek/write file: %s\n", Tmptwo);
		exit(1);
	}

	random(1001);
	for(i = 0, rv = 0; i < ssize; ++i)
	{	size_t	s, r, bound;
		long	p;

#define BOUND	(SMALL*SMALL + 1)
		p = (long)(random(0) % (bsize - BOUND));
		fseek(fp, p, SEEK_SET);
		fseek(fw, p, SEEK_SET);

		bound = (random(0) % BOUND) + SMALL;
		for(v = 0; v < bound; )
		{	s = (random(0) % 9) + 8;
			if(s > (bound-v) )
				s = bound-v;

			if((r = fread(data, 1, s, fp)) <= 0)
				break;
			rv += r;

			if((r = fwrite(data, 1, r, fw)) <= 0)
				break;
			rv += r;

			v += r;
		}

		if(v <= 0)
			error("fseek/fread/fwrite failed\n", v);
	}
	fclose(fp);
	fclose(fw);
	ertm = times(&etm);

	report("sksmall",i,rv,ertm-brtm,&btm,&etm,'S');
#endif

#if COPY /* copy data */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening copyread file: %s\n", Tmpone);
		exit(1);
	}
	if(!(fw = fopen(Tmptwo,"wb")) )
	{	fprintf(stderr,"Error opening copywrite file: %s\n", Tmptwo);
		exit(1);
	}
	for(i = 0;;)
	{	if((rv = fread(data,1,sizeof(data),fp)) <= 0)
			break;
		else	i += rv;

		if((v = fwrite(data,1,rv,fw)) != rv)
		{	fprintf(stderr,"Error copy fwrite: fw=%d i=%d\n",v,i);
			break;
		}
		else	i += v;
	}
	fclose(fp);
	fclose(fw);

	ertm = times(&etm);

	report("copy",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if SFMOVE /* sfmove */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening copyread file: %s\n", Tmpone);
		exit(1);
	}
	if(!(fw = fopen(Tmptwo,"wb")) )
	{	fprintf(stderr,"Error opening copywrite file: %s\n", Tmptwo);
		exit(1);
	}
#if _SFIO_H
	i = (int)sfmove(fp,fw,(Sfoff_t)SF_UNBOUND,-1);
	sfsync(fw);
#else
	i = 0;
#endif
	i *= 2;
	fclose(fp);
	fclose(fw);

	ertm = times(&etm);

	report("sfmove",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if PUTC /* sequential putc */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"wb")) )
	{	fprintf(stderr,"Error opening putc file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0; i < csize; ++i)
	{	if(putc(i&0377,fp) < 0)
		{	fprintf(stderr,"Error fputc i=%d\n", i);
			break;
		}
	}
	fclose(fp);

	ertm = times(&etm);

	report("putc",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if GETC /* sequential getc */
	brtm = times(&btm);
	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening getc file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0; i < csize; ++i)
		if(getc(fp) < 0)
			break;
	fclose(fp);
	ertm = times(&etm);
	report("getc",i/1024,i,ertm-brtm,&btm,&etm,'K');
#endif

#if PUTS /* fputs */
	line = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\n";
	len = strlen(line);

	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"wb")) )
	{	fprintf(stderr,"Error opening puts file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0; i < lsize; ++i)
		if(fputs(line,fp) < 0)
			break;
	fclose(fp);

	ertm = times(&etm);

	report("puts",i,i*strlen(line),ertm-brtm,&btm,&etm,'L');
#endif

#if GETS /* fgets */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening gets file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0;; ++i)
		if(!fgets(data,sizeof(data),fp))
			break;
	fclose(fp);

	ertm = times(&etm);

	report("gets",i,i*len,ertm-brtm,&btm,&etm,'L');
#endif

#if SFGETR /* sfgetr */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening sfgetr file: %s\n", Tmpone);
		exit(1);
	}
#if _SFIO_H
	for(i = 0;; ++i)
		if(!sfgetr(fp,'\n',0))
			break;
#else
	i = 0;
#endif
	fclose(fp);

	ertm = times(&etm);

	report("sfgetr",i,i*len,ertm-brtm,&btm,&etm,'L');
#endif

#if REVGETS /* reverse fgets */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening revgets file: %s\n", Tmpone);
		exit(1);
	}
	for(rv = 0, i = 0; i < lsize; ++i)
	{	if(fseek(fp,(lsize-(i+1))*len,0) < 0)
			break;
		if(!fgets(data,sizeof(data),fp))
			break;
		rv += len;
	}
	fclose(fp);

	ertm = times(&etm);

	report("revgets",i,rv,ertm-brtm,&btm,&etm,'L');
#endif

#if PRINTF /* fprintfs */
	lsize /= 4;

	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"wb")) )
	{	fprintf(stderr,"Error opening fprintf file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0; i < lsize; ++i)
		fprintf(fp,"%c\t%d\t%o\t%x\t%f\t%e\t%s\n",
			'a'+(i%26),i,i,i,(i*11.0),(i*11.0),"abcdefghijklmnopqrstuvwxyz");

	rv = (int)ftell(fp);
	fclose(fp);

	ertm = times(&etm);

	report("printf",i,rv,ertm-brtm,&btm,&etm,'L');
#endif

#if SCANF /* fscanfs */
	brtm = times(&btm);

	if(!(fp = fopen(Tmpone,"rb")) )
	{	fprintf(stderr,"Error opening fscanf file: %s\n", Tmpone);
		exit(1);
	}
	for(i = 0, rv = 0; i < lsize; ++i)
	{	char	c;
		int	d, o, x;
		float	f, e;

		rv = fscanf(fp,"%c\t%d\t%o\t%x\t%f\t%e\t%s\n",&c,&d,&o,&x,&f,&e,data);
		if(rv != 7)
			break;
	}
	rv = (int)ftell(fp);
	fclose(fp);
	ertm = times(&etm);

	report("scanf",i,rv,ertm-brtm,&btm,&etm,'L');
#endif

	endrtm = times(&endtm);
	report("elapse",0,0,endrtm-begrtm,&begtm,&endtm,'E');

#ifndef DEBUG
	unlink(Tmpone);
	unlink(Tmptwo);
#endif

	return 0;
}
