#include	<signal.h>
#include	<sys/types.h>
#include	<sys/times.h>

#ifndef TIC_SEC
#define TIC_SEC	60
#endif

#ifdef __STDC__
#define _A_(x)	x
#else
#define _A_(x)	()
#endif

extern void	exit _A_((int));
extern int	unlink _A_((char*));
extern int	rand _A_((void));

#include	<stdio.h>

/*	Program to benchmark/exercise various primitives in stdio.
**	Called as:
**	stdiobm [-c#ofKbytes] [-l#ofLines] -s[#ofSeeks]
**		-c: This option affects the tests for fread(),
**		    fwrite(), putc() and getc(). It specifies
**		    the number of Kbytes to do io. For example,
**		    -c1024 will do i/o of a megabyte. Putc/getc
**		    will only do half of this size.
**		-l: This option affects the tests for gets(),
**		    puts(), printf() and scanf(). It specifies
**		    the number of times that each of these functions
**		    will be called (number of lines generated).
**		-s: This option affects the test for fseek()+fread()+fwrite().
**		    It specifies the number of times to do this combination.
**		-v: Prints explanation of the statistics.
**
**	Written by Kiem-Phong Vo (12/11/90)
*/

char	*Mesg[] =
{
"	Benchmark Tests:\n\n",
"fwrite:		write a number of 8K-blocks\n",
"fread:		read a number of 8K-blocks\n",
"revrd:		read 8K-blocks in reverse order\n",
"fw757:		write blocks of 757 bytes\n",
"fr757:		read blocks of 757 bytes\n",
"rev757:		read blocks of 757 bytes in reverse order\n",
"copy&rw:	copy from one file to another using fread/fwrite\n",
"seek+rw:	copy a block from a random location to 0\n",
"putc:		write a byte at a time\n",
"getc:		read a byte at a time\n",
"fputs:		write lines\n",
"fgets:		read lines\n",
"revgets:	read lines in reverse order\n",
"fprintf:	print lines using %d,%o,%x,%f,%e,%c,%s\n",
"fscanf:		read data using %d,%o,%x,%f,%e,%c,%s\n",
"\n",
(char*)0
};

#define reg	register

char	Tmpfile[64];	/* the primary temp file that we'll do io on */
char	Cpfile[64];	/* for copying data */

/* quit for any exception */
#ifdef __STDC__
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
#ifdef __STDC__
static void sigproc(int sig)
#else
static void sigproc(sig)
int	sig;
#endif
{	if(Tmpfile[0])
		unlink(Tmpfile);
	if(Cpfile[0])
		unlink(Cpfile);
	error("Quit on signal %d\n",sig);
}

/* print a report */
#ifdef __STDC__
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
	reg long	user, sys;
	static int	first = 1;

	if(first)
	{	first = 0;
		fprintf(stdout,"func\treal\tuser\tsys\tu+s\tsize\tKbytes/s\n");
	}

	user = etm->tms_utime - btm->tms_utime;
	sys  = etm->tms_stime - btm->tms_stime;
	if((user+sys) <= 0)
		bytes = -1;
	else	bytes = (int)(((bytes/1024.)*TIC_SEC)/(user+sys));
	fprintf(stdout,"%s\t%.2f\t%.2f\t%.2f\t%.2f\t%d%c\t%d\n",
		func,
		real/((float)TIC_SEC),
		user/((float)TIC_SEC),
		sys/((float)TIC_SEC),
		(user+sys)/((float)TIC_SEC),
		n,
		type,
		bytes);
	fflush(stdout);
}

#ifdef __STDC__
main(int argc, char** argv)
#else
main(argc,argv)
int	argc;
char	**argv;
#endif
{
	reg FILE	*fp, *fw;
	reg int		i, rv, len;
	char		data[8192];
	struct tms	btm, etm, begtm, endtm;
	long		brtm, ertm, begrtm, endrtm;
	int		ssize = 2000, csize = 10000, lsize = 50000;
	char		*line;

	while(argc > 1 && argv[1][0] == '-')
	{	switch(argv[1][1])
		{
		case 'c' :
			if((csize = atoi(argv[1]+2)) < 0)
				csize = 10000;
			break;
		case 'l' :
			if((lsize = atoi(argv[1]+2)) < 0)
				lsize = 50000;
			break;
		case 's' :
			if((ssize = atoi(argv[1]+2)) < 0)
				ssize = 2000;
			break;
		case 'v' :
			for(i = 0; Mesg[i]; ++i)
				fputs(Mesg[i],stdout);
			break;
		}
		argc--;
		argv++;
	}

	/* do this many megs of byte pushing */
	csize *= 1024;

	/* create a temp file name that we can use */
	strcpy(Tmpfile,"/tmp/kpvxxx");
	strcpy(Cpfile,"/tmp/kpvyyy");

	/* quit on the following signals */
        signal(SIGINT,sigproc);
        signal(SIGHUP,sigproc);
        signal(SIGQUIT,sigproc);

	begrtm = times(&begtm);

	/* block writes */
	for(i = sizeof(data); i != 0; --i)
		data[i] = i&0377;
	fp = fopen(Tmpfile,"w");
	brtm = times(&btm);
	for(i = 0; i < csize; )
	{	rv = fwrite(data,1,sizeof(data),fp);
		if(rv <= 0)
		{	fprintf(stderr,"fwrite: %d\n",rv);
			break;
		}
		i += rv;
	}
	fflush(fp);
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fwrite",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* block reads */
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	for(i = 0;;)
	{	rv = fread(data,1,sizeof(data),fp);
		if(rv <= 0)
		{	if(rv < 0)
				fprintf(stderr,"fread: %d\n",rv);
			break;
		}
		i += rv;
	}
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fread",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* reverse block reads */
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	fseek(fp,-((long)sizeof(data)),2);
	for(i = 0;;)
	{	rv = fread(data,1,sizeof(data),fp);
		if(rv <= 0)
		{	if(rv < 0)
				fprintf(stderr,"revread: %d\n",rv);
			break;
		}
		i += rv;
		if(fseek(fp,-2*((long)sizeof(data)),1) < 0)
			break;
	}
	ertm = times(&etm);
	rv = csize - (int)ftell(fp);
	fclose(fp);
	report("revrd",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* write by 757b */
	fp = fopen(Tmpfile,"w");
	brtm = times(&btm);
	for(i = 0; i < csize; )
	{	
		rv = fwrite(data,1,757,fp);
		if(rv <= 0)
		{	fprintf(stderr,"fr757: %d\n",rv);
			break;
		}
		i += rv;
	}
	fflush(fp);
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fw757",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* read by 757b */
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	for(i = 0;;)
	{	rv = fread(data,1,757,fp);
		if(rv <= 0)
		{	if(rv < 0)
				fprintf(stderr,"fr757: %d\n",rv);
			break;
		}
		i += rv;
	}
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fr757",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* reverse read by 757b */
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	fseek(fp,-757L,2);
	for(i = 0;;)
	{	rv = fread(data,1,757,fp);
		if(rv <= 0)
		{	if(rv < 0)
				fprintf(stderr,"rev757: %d\n",rv);
			break;
		}
		i += rv;
		if(fseek(fp,-2*757L,1) < 0)
			break;
	}
	ertm = times(&etm);
	rv = csize - (int)ftell(fp);
	fclose(fp);
	report("rev757",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* copy data */
	fp = fopen(Tmpfile,"r");
	fw = fopen(Cpfile,"w");
	brtm = times(&btm);
	for(i = 0;;)
	{	rv = fread(data,1,sizeof(data),fp);
		if(rv <= 0)
		{	if(rv < 0)
				fprintf(stderr,"copy rd: %d\n",rv);
			break;
		}
		if(rv > 0)
			i += rv;
		if(fwrite(data,1,rv,fw) != rv)
		{	fprintf(stderr,"copy wr: %d\n",rv);
			break;
		}
	}
	fflush(fw);
	rv = (int) ftell(fw);
	ertm = times(&etm);
	fclose(fp);
	fclose(fw);
	report("copy&rw",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* mixing seek/read/write */
	csize -= sizeof(data);
	fp = fopen(Tmpfile,"r+");
	brtm = times(&btm);
	for(i = 0, rv = 0; i < ssize; ++i)
	{	reg long	s;
		if((s = rand()%csize) < 0)
			s = 0;
		if(fseek(fp,s,0) < 0)
			break;
		if(fread(data,1,sizeof(data),fp) != sizeof(data))
			break;
		if(fseek(fp,0L,0) < 0)
			break;
		if(fwrite(data,1,sizeof(data),fp) != sizeof(data))
			break;
		rv += sizeof(data);
	}
	fflush(fp);
	ertm = times(&etm);
	fclose(fp);
	report("seek+rw",i,rv,ertm-brtm,&btm,&etm,'S');
	csize += sizeof(data);

	/* sequential putc */
	csize /= 2;
	fp = fopen(Tmpfile,"w");
	brtm = times(&btm);
	for(i = 0; i < csize; ++i)
		if(putc(i&0377,fp) < 0)
		{	fprintf(stderr,"fputc\n");
			break;
		}
	fflush(fp);
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("putc",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* sequential getc */
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	for(i = 0; i < csize; ++i)
		if(getc(fp) < 0)
			break;
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("getc",i/1024,rv,ertm-brtm,&btm,&etm,'K');

	/* fputs */
	line = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\n";
	len = strlen(line);
	fp = fopen(Tmpfile,"w");
	brtm = times(&btm);
	for(i = 0; i < lsize; ++i)
		fputs(line,fp);
	fflush(fp);
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fputs",i,rv,ertm-brtm,&btm,&etm,'L');

	/* fgets */
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	for(i = 0;; ++i)
		if(!fgets(data,sizeof(data),fp))
			break;
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fgets",i,rv,ertm-brtm,&btm,&etm,'L');

	/* reverse fgets */
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	for(rv = 0, i = 0; i < lsize; ++i)
	{	if(fseek(fp,(lsize-(i+1))*len,0) < 0)
			break;
		if(!fgets(data,sizeof(data),fp))
			break;
		rv += len;
	}
	ertm = times(&etm);
	fclose(fp);
	report("revgets",i,rv,ertm-brtm,&btm,&etm,'L');

	/* fprintfs */
	fp = fopen(Tmpfile,"w");
	brtm = times(&btm);
	for(i = 0; i < lsize; ++i)
		fprintf(fp,"%c\t%d\t%o\t%x\t%f\t%e\t%s\n",
			'a'+(i%26),i,i,i,(i*11.0),(i*11.0),"abcdefghijklmnopqrstuvwxyz");
	fflush(fp);
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fprintf",i,rv,ertm-brtm,&btm,&etm,'L');

	/* fscanfs */
	etm = btm;
	i = rv = 0;
	fp = fopen(Tmpfile,"r");
	brtm = times(&btm);
	for(i = 0; i < lsize; ++i)
	{	char	c;
		int	d, o, x;
		double	f, e;

		rv = fscanf(fp,"%c\t%d\t%o\t%x\t%f\t%e\t%s\n",&c,&d,&o,&x,&f,&e,data);
		if(rv != 7)
			break;
	}
	ertm = times(&etm);
	rv = (int)ftell(fp);
	fclose(fp);
	report("fscanf",i,rv,ertm-brtm,&btm,&etm,'L');

	endrtm = times(&endtm);
	report("elapse",0,0,endrtm-begrtm,&begtm,&endtm,'E');

	unlink(Tmpfile);
	unlink(Cpfile);
	return 0;
}
