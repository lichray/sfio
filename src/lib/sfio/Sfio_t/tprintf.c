#include	"sftest.h"

static char	Buf[128];

typedef struct _coord_
{	int	x;
	int	y;
} Coord_t;

Coord_t	Coord;

#if __STD_C
coordarg(Sfio_t* f, Void_t* val, Sffmt_t* fe)
#else
coordarg(f,val,fe)
Sfio_t*		f;
Void_t*		val;
Sffmt_t*	fe;
#endif
{	
	*((Coord_t**)val) = &Coord;
	return 1;
}

#if __STD_C
abCprint(Sfio_t* f, Void_t* v, int width, Sffmt_t* fe)
#else
abCprint(f, v, width, fe)
Sfio_t*		f;
Void_t*		v;
int		width;
Sffmt_t*	fe;
#endif
{
	Coord_t*	cp;
	int		p;
	char		mytype[128];

	switch(fe->fmt)
	{
	case 'a' :
		if(fe->base > 0)
			sfsprintf(Buf,sizeof(Buf),"%..*u",fe->base,(unsigned int)v);
		else	sfsprintf(Buf,sizeof(Buf),"%u",(unsigned int)v);
		fe->t_str = Buf;
		fe->n_str = sfslen();
		return 1;
	case 'b' :
		if(fe->base > 0)
			sfsprintf(Buf,sizeof(Buf),"%..*d",fe->base,(int)v);
		else	sfsprintf(Buf,sizeof(Buf),"%d",(int)v);
		fe->t_str = Buf;
		fe->n_str = sfslen();
		return 1;
	case 'C' :
		cp = (Coord_t*)v;
		for(p = 0; ; ++p)
			if(fe->t_str[p] != '(' )
				break;
		fe->t_str += p;
		fe->n_str -= 2*p;
		memcpy(mytype,fe->t_str,fe->n_str); mytype[fe->n_str] = 0;
		sfsprintf(Buf,sizeof(Buf),mytype,cp->x,cp->y);
		fe->t_str = Buf;
		fe->n_str = sfslen();
		return 1;
	case 'z' : /* test return value of extension function */
		fe->t_str = (char*)v;
		fe->n_str = 10;
		return 1;
	case 'Z' : /* terminate format processing */
	default :
		return -1;
	}
}

#if __STD_C
intarg(Sfio_t* f, Void_t* val, Sffmt_t* fe)
#else
intarg(f, val, fe)
Sfio_t*		f;
Void_t*		val;
Sffmt_t*	fe;
#endif
{	static int	i = 1;
	*((int*)val) = i++;
	return 1;
}


#if __STD_C
void stkprint(char* buf, int n, char* form, ...)
#else
void stkprint(buf,n,form,va_alist)
char*	buf;
int	n;
char*	form;
va_dcl
#endif
{	va_list	args;
	Sffmt_t	fe;
#if __STD_C
	va_start(args,form);
#else
	va_start(args);
#endif
	fe.form = form;
	va_copy(fe.args,args);
	fe.argf = NIL(Sfarg_f);
	fe.extf = NIL(Sfext_f);
	sfsprintf(buf,n,"%! %d %d",&fe,3,4);
	va_end(args);
}

main()
{
	char	buf1[1024], buf2[1024], *list[4], *s;
	float	x=0.0051;
	int	i, j;
	Sffmt_t	fe;

	sfsprintf(buf1,sizeof(buf1),"%6.2f",x);
	if(strcmp(buf1,"  0.01") != 0)
		terror("%%f rounding wrong\n");

	fe.form = NIL(char*);
	fe.argf = NIL(Sfarg_f);
	fe.extf = abCprint;
	sfsprintf(buf1,sizeof(buf1),"%..4u %..4d9876543210",-1,-1);
	sfsprintf(buf2,sizeof(buf2),"%!%..4a %..4b%z%Zxxx",
			&fe,-1,-1,"98765432109876543210",0);
	if(strcmp(buf1,buf2) != 0)
		terror("%%!: Extension function failed\n");

	fe.argf = intarg;
	fe.extf = NIL(Sfext_f);
	sfsprintf(buf1,sizeof(buf1),"%d %d",1,2);
	sfsprintf(buf2,sizeof(buf2),"%!%d %d",&fe);
	if(strcmp(buf1,buf2) != 0)
		terror("%%!: Getarg function failed\n");

	Coord.x = 5;
	Coord.y = 7;
	sfsprintf(buf1,sizeof(buf1),"%d %d",Coord.x,Coord.y);

	fe.argf = NIL(Sfarg_f);
	fe.extf = abCprint;
	sfsprintf(buf2,sizeof(buf2),"%!%((%d %d))C",&fe,&Coord);
	if(strcmp(buf1,buf2) != 0)
		terror("%%()C failed\n");

	fe.argf = coordarg;
	sfsprintf(buf2,sizeof(buf2),"%!%((%d %d))C",&fe);
	if(strcmp(buf1,buf2) != 0)
		terror("%%()C failed2\n");

	sfsprintf(buf1,sizeof(buf1),"%d %d %d %d",1,2,3,4);
	stkprint(buf2,sizeof(buf2),"%d %d",1,2);
	if(strcmp(buf1,buf2) != 0)
		terror("%%!: Stack function failed\n");

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

	if(sizeof(int) == 4 &&  sizeof(short) == 2)
	{	char* s = sfprints("%hx",0xffffffff);
		if(!s || strcmp(s,"ffff") != 0)
			terror("Failed %%hx test\n");
	}

	sfsprintf(buf1,sizeof(buf1),"%#..16d",-0xabc);
	if(strcmp(buf1,"-16#abc") != 0)
		terror("Failed %%..16d test\n");

	sfsprintf(buf1,sizeof(buf1),"%#..16lu",(long)0xc2c01576);
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

	list[0] = "0";
	list[1] = "1";
	list[2] = "2";
	list[3] = 0;
	sfsprintf(buf1,sizeof(buf1),"%..*s", ',', list);
	if(strcmp(buf1,"0,1,2") != 0)
		terror("Failed %%..*s test\n");

	sfsprintf(buf1,sizeof(buf1),"%.2.*c", ',', "012");
	if(strcmp(buf1,"00,11,22") != 0)
		terror("Failed %%..*c test\n");

#if _typ_long_long
	{ long long	ll;
	  char* s = sfprints("%#..16llu",~((long long)0));
	  sfsscanf(s,"%lli", &ll);
	  if(ll != (~((long long)0)) )
		terror("Failed inverting printf/scanf long long1\n");

	  s = sfprints("%#..18lld",~((long long)0));
	  sfsscanf(s,"%lli", &ll);
	  if(ll != (~((long long)0)) )
		terror("Failed inverting printf/scanf long long2\n");

	  s = sfprints("%#..lli",~((long long)0));
	  sfsscanf(s,"%lli", &ll);
	  if(ll != (~((long long)0)) )
		terror("Failed inverting printf/scanf long long3\n");
	}
#endif

	i = (int)(~(~((uint)0) >> 1));
	s = sfprints("%d",i);
	j = atoi(s);
	if(i != j)
		terror("Failed converting highbit\n");
	
	for(i = -10000; i < 10000; i += 123)
	{	s = sfprints("%d",i);
		j = atoi(s);
		if(j != i)
			terror("Failed integer conversion\n");
	}

	return 0;
}
