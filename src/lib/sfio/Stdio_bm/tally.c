#include	<sfio.h>

typedef struct	line_s
{	char	func[16];
	double	real;
	double	user;
	double	sys;
	double	us;
	int	size;
	char	c;
	int	kbyte;
} Line_t;


#if __STD_C
int compare(const void* e1, const void* e2)
#else
int compare(e1,e2)
void*	e1;
void*	e2;
#endif
{	Line_t	*d1 = (Line_t*)e1, *d2 = (Line_t*)e2;
	return d1->us < d2->us ? -1 : d1->us == d2->us ? 0 : 1;
}

extern qsort _ARG_((void*, size_t, size_t, int(*)(const void*,const void*)));

#if __STD_C
main(int argc, char** argv)
#else
main(argc,argv)
int	argc;
char	**argv;
#endif
{
	Sfio_t	*f[10];
	int	i, n;
	char	*str;
	Line_t	line[10];
	double	r, u, s, us, r_all, u_all, s_all, r_norev, u_norev, s_norev, us_all, us_norev;
	int	kbyte;

	for(n = 0; n < argc-1; ++n)
	{	if(!(f[n] = sfopen((Sfio_t*)0,argv[n+1],"r")))
			return -1;
		str = sfgetr(f[n],'\n',1);
	}

	r_norev = r_all = u_all = s_all = us_all = u_norev = s_norev = us_norev = 0;

	sfprintf(sfstdout,"func\tsize\treal\tuser\tsys\tu+s\tKbs/sec\n");
	while((str = sfgetr(f[0],'\n',1)) != (char*)0)
	{	sfsscanf(str,"%s%lf%lf%lf%lf%d%c%d",
			line[0].func,
			&line[0].real,
			&line[0].user,
			&line[0].sys,
			&line[0].us,
			&line[0].size,
			&line[0].c,
			&line[0].kbyte);
		for(i = 1; i < n; ++i)
		{	if(!(str = sfgetr(f[i],'\n',1)))
				return;
			sfsscanf(str,"%s%lf%lf%lf%lf%d%c%d",
				line[i].func,
				&line[i].real,
				&line[i].user,
				&line[i].sys,
				&line[i].us,
				&line[i].size,
				&line[i].c,
				&line[i].kbyte);
		}

		/*qsort(line,n,sizeof(line[0]),compare);*/
		s = u = us = r = 0;
		kbyte = 0;
		for(i = 0; i < n; ++i)
		{	us += line[i].sys + line[i].user;
			s += line[i].sys;
			u += line[i].user;
			r += line[i].real;
			kbyte += line[i].kbyte;
		}
		us /= n;
		s  /= n;
		u  /= n;
		r  /= n;
		kbyte /= n;
		if(us > 0)
			kbyte /= us;
		else	kbyte = 0;

		if(strcmp(line[0].func,"elapse") == 0)
			continue;
		if(strcmp(line[0].func,"sfmove") != 0 &&
		   strcmp(line[0].func,"sfgetr") != 0 )
		{	if(strncmp(line[0].func,"rev",3) != 0)
			{	us_norev += us;
				s_norev += s;
				u_norev += u;
				r_norev += r;
			}
			us_all += us;
			s_all += s;
			u_all += u;
			r_all += r;
		}
		
		sfprintf(sfstdout,"%s\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%d\n",
			line[0].func, line[0].size, r, u, s, us, kbyte);
	}

	sfprintf(sfstdout,"%s\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%d\n",
		 "norev", 0, r_norev, u_norev, s_norev, us_norev, 0);
	sfprintf(sfstdout,"%s\t%d\t%.2f\t%.2f\t%.2f\t%.2f\t%d\n",
		 "all", 0, r_all, u_all, s_all, us_all, 0);
}
