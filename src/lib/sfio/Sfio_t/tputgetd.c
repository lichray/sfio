#include	"sftest.h"

#if __STD_C
main(void)
#else
main()
#endif
{
	double	f, v;
	Sfio_t	*fp;

	if(!(fp = sftmp(8)))
		terror("Can't open temp file\n");

	for(f = 1e-10; f < 1e-10+1.; f += .001)
		if(sfputd(fp,f) < 0)
			terror("Writing %f\n",f);

	sfseek(fp,0L,0);
	for(f = 1e-10; f < 1e-10+1.; f += .001)
		if((v = sfgetd(fp)) != f)
			terror("Input=%f, Expect=%f\n",v,f);
	return 0;
}
