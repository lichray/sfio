#include	"sftest.h"
#include	<values.h>

main()
{
	char	str[8], c[4], cl[8];
	int	i, j, k, n;
	float	f;
	double	d;
	char*	s;
	Sfio_t*	sf;

	str[0] = str[1] = str[2] = str[3] =
	str[4] = str[5] = str[6] = str[7] = 'x';
	c[0] = c[1] = c[2] = c[3] = 'x';
	cl[0] = cl[1] = cl[2] = cl[3] =
	cl[4] = cl[5] = cl[6] = cl[7] = 'x';

	sfsscanf("1234567890","%4#s%2#c%4#[0-9]",str,4,c,2,cl,6);

	if(strcmp(str,"123") != 0)
		terror("Bad s\n");
	if(str[4] != 'x')
		terror("str overwritten\n");

	if(strncmp(c,"56",2) != 0)
		terror("Bad c\n");
	if(c[2] != 'x')
		terror("c overwritten\n");

	if(strcmp(cl,"7890") != 0)
		terror("Bad class\n");
	if(cl[5] != 'x')
		terror("cl overwritten\n");

	if(sfsscanf("123 ab","%*d") != 0)
		terror("Bad return value\n");

	if(sfsscanf("123abcA","%[0-9]%[a-z]%[0-9]",str,c,cl) != 2 ||
	   strcmp(str,"123") != 0 || strcmp(c,"abc") != 0)
		terror("Bad character class scanning\n");

	if(sfsscanf("123 456 ","%d %d%n",&i,&j,&n) != 2)
		terror("Bad integer scanning\n");
	if(i != 123 || j != 456 || n != 7)
		terror("Bad return values\n");

	if(sfsscanf("1 2","%d %d%n",&i,&j,&n) != 2)
		terror("Bad scanning2\n");
	if(i != 1 || j != 2 || n != 3)
		terror("Bad return values 2\n");

	if(sfsscanf("1234 1","%2d %d%n",&i,&j,&n) != 2)
		terror("Bad scanning3\n");
	if(i != 12 || j != 34 || n != 4)
		terror("Bad return values 3\n");

	if(sfsscanf("011234 1","%3i%1d%1d%n",&i,&j,&k,&n) != 3)
		terror("Bad scanning4\n");
	if(i != 9 || j != 2 || k != 3 || n != 5)
		terror("Bad return values 4\n");

	if(sfsscanf("4 6","%f %lf",&f, &d) != 2)
		terror("Bad scanning5\n");
	if(f != 4 || d != 6)
		terror("Bad return values 5\n");

#if _lib_locale
	{	int		decpoint = 0;
		struct lconv*	lv;
		GETDECIMAL(decpoint,lv);
		s = sfprints("%c1234 %c1234",decpoint,decpoint);
	}
#else
		s = ".1234 .1234";
#endif
	if(sfsscanf(s,"%f %lf",&f, &d) != 2)
		terror("Bad scanning6\n");

	if(f <= .1233 || f >= .1235 || d <= .1233 || d >= .1235)
		terror("Bad return values: f=%.4f d=%.4lf\n",f,d);

	/* test for scanning max double value */
	s = sfprints("%.14le",MAXDOUBLE);
	if(!s || s[0] < '0' || s[0] > '9')
		terror("sfprints failed\n");
	for(i = 0; s[i]; ++i)
		if(s[i] == 'e')
			break;
	if(s[i-1] > '0' && s[i-1] <= '9')
		s[i-1] -= 1;
	sfsscanf(s,"%le",&d);
	if(d > MAXDOUBLE || d < MAXDOUBLE/2)
		terror("sfscanf of MAXDOUBLE failed\n");

	if(!(sf = sftmp(8*1024)) )
		terror("Opening temp file\n");

	for(k = 2; k <= 64; ++k)
	{	sfseek(sf,(Sfoff_t)0,0);
		for(i = 0; i < 1000; ++i)
			sfprintf(sf,"%#..*d\n",k,i);
		sfseek(sf,(Sfoff_t)0,0);
		for(i = 0; i < 1000; ++i)
		{	if(sfscanf(sf,"%i",&j) != 1)
				terror("Scanf failed\n");
			if(i != j)
				terror("Wrong scanned value\n");
		}
	}

	if(sfsscanf("2#1001","%i",&i) != 1 || i != 9)
		terror("Bad %%i scanning\n");
	if(sfsscanf("2#1001","%#i%c",&i,c) != 2 || i != 2 || c[0] != '#')
		terror("Bad %%#i scanning\n");

	return 0;
}
