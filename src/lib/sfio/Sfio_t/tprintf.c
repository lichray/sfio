#include	"sftest.h"

static char	buf[128];

typedef struct _coord_
{	int	x;
	int	y;
} Coord_t;

Coord_t	Coord;

#if __STD_C
coordarg(int fmt, char* val, char* type, int n)
#else
coordarg(fmt,val,type,n)
int	fmt;
char*	val;
char*	type;
int	n;
#endif
{	
	*((Coord_t**)val) = &Coord;
	return 0;
}

#if __STD_C
abCprint(char* v, int fmt, int precis, char** rets, int base, char* type, int n)
#else
abCprint(v, fmt, precis, rets, base, type, n)
char*	v;
int	fmt;
int	precis;
char**	rets;
int	base;
char*	type;
int	n;
#endif
{
	Coord_t*	cp;
	int		p;
	char		mytype[128];

	switch(fmt)
	{
	case 'a' :
		if(base > 0)
			sfsprintf(buf,sizeof(buf),"%..*u",base,(unsigned int)v);
		else	sfsprintf(buf,sizeof(buf),"%u",(unsigned int)v);
		*rets = buf;
		return strlen(buf);
	case 'b' :
		if(base > 0)
			sfsprintf(buf,sizeof(buf),"%..*d",base,(int)v);
		else	sfsprintf(buf,sizeof(buf),"%d",(int)v);
		*rets = buf;
		return strlen(buf);
	case 'C' :
		cp = (Coord_t*)v;
		for(p = 0; ; ++p)
			if(type[p] != '(' )
				break;
		type += p;
		n -= 2*p;
		memcpy(mytype,type,n); mytype[n] = 0;
		sfsprintf(buf,sizeof(buf),mytype,cp->x,cp->y);
		*rets = buf;
		return strlen(buf);
	case 'Z' : /* terminate format processing */
		*rets = NIL(char*);
		return -1;
	default :
		return -1;
	}
}

#if __STD_C
intarg(int fmt, char* val, char* type, int n)
#else
intarg(fmt,val,type,n)
int	fmt;
char*	val;
char*	type;
int	n;
#endif
{	static int	i = 1;
	*((int*)val) = i++;
	return 0;
}


#if __STD_C
void stkprint(char* buf, int n, char* form, ...)
#else
void stkprint(buf,n,form,va_alist)
char	*buf;
int	n;
char	*form;
va_dcl
#endif
{	va_list	args;
#if __STD_C
	va_start(args,form);
#else
	va_start(args);
#endif
	sfsprintf(buf,n,"%: %d %d",form,&args,3,4);
	va_end(args);
}

#if __STD_C
main(void)
#else
main()
#endif
{
	char	buf1[1024], buf2[1024];
	float	x=0.0051;

	sfsprintf(buf1,sizeof(buf1),"%6.2f",x);
	if(strcmp(buf1,"  0.01") != 0)
		terror("%%f rounding wrong\n");

	sfsprintf(buf1,sizeof(buf1),"%..4u %..4d",-1,-1);
	sfsprintf(buf2,sizeof(buf2),"%&%..4a %..4b%Zxxx",abCprint,-1,-1,0);
	if(strcmp(buf1,buf2) != 0)
		terror("%%&: Extension function failed\n");

	sfsprintf(buf1,sizeof(buf1),"%d %d",1,2);
	sfsprintf(buf2,sizeof(buf2),"%@%d %d",intarg);
	if(strcmp(buf1,buf2) != 0)
		terror("%%@: Getarg function failed\n");

	Coord.x = 5;
	Coord.y = 7;
	sfsprintf(buf1,sizeof(buf1),"%d %d",Coord.x,Coord.y);

	sfsprintf(buf2,sizeof(buf2),"%&%((%d %d))C",abCprint,&Coord);
	if(strcmp(buf1,buf2) != 0)
		terror("%%()C failed\n");

	sfsprintf(buf2,sizeof(buf2),"%&%@%((%d %d))C",abCprint,coordarg);
	if(strcmp(buf1,buf2) != 0)
		terror("%%()C failed2\n");

	sfsprintf(buf1,sizeof(buf1),"%d %d %d %d",1,2,3,4);
	stkprint(buf2,sizeof(buf2),"%d %d",1,2);
	if(strcmp(buf1,buf2) != 0)
		terror("%%:: Stack function failed\n");

	sfsprintf(buf1,sizeof(buf1),"% +G",-1.2345);
#if _lib_locale
	{	int		decpoint = 0;
		struct lconv*	lv;
		GETDECIMAL(decpoint,lv);
		sfsprintf(buf2,sizeof(buf2),"-1%c2345",decpoint);
	}
#else
		sfsprintf(buf2,sizeof(buf2),"-1.2345");
#endif
	if(strcmp(buf1,buf2) != 0)
		terror("Failed %% +G test\n");

	sfsprintf(buf1,sizeof(buf1),"%#..16d",-0xabc);
	if(strcmp(buf1,"-16#abc") != 0)
		terror("Failed %%..16d test\n");

	sfsprintf(buf1,sizeof(buf1),"%#..16lu",0xc2c01576);
	if(strcmp(buf1,"16#c2c01576") != 0)
		terror("Failed %%..16u test\n");

	sfsprintf(buf1,sizeof(buf1),"%0#4o",077);
	if(strcmp(buf1,"0077") != 0)
		terror("Failed %%0#4o test\n");

	sfsprintf(buf1,sizeof(buf1),"%0#4x",0xc);
	if(strcmp(buf1,"0x000c") != 0)
		terror("Failed %%0#4x test\n");

	sfsprintf(buf1,sizeof(buf1),"%c%c%c",'a','b','c');
	if(strcmp(buf1,"abc") != 0)
		terror("Failed %%c test\n");

	sfsprintf(buf1,sizeof(buf1),"%.4c",'a');
	if(strcmp(buf1,"aaaa") != 0)
		terror("Failed %%.4c test\n");

	sfsprintf(buf1,sizeof(buf1),"%hd%c%hd%ld",(short)1,'1',(short)1,1L);
	if(strcmp(buf1,"1111") != 0)
		terror("Failed %%hd test\n");

	sfsprintf(buf1,sizeof(buf1),"%10.5E",(double)0.0000625);
	if(strcmp(buf1,"6.25000E-05") != 0)
		terror("Failed %%E test\n");

	sfsprintf(buf1,sizeof(buf1),"%10.5f",(double)0.0000625);
	if(strcmp(buf1,"   0.00006") != 0)
		terror("Failed %%f test\n");

	sfsprintf(buf1,sizeof(buf1),"%10.5G",(double)0.0000625);
	if(strcmp(buf1,"  6.25E-05") != 0)
		terror("Failed %%G test\n");

	return 0;
}
