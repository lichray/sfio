#include	"sftest.h"

main()
{
	double	f, v;
	int	i;
	Sfio_t	*fp;

	if(!(fp = sfopen(NIL(Sfio_t*),"xxx","w+")) )
		terror("Can't open temp file\n");

	for(f = 1e-10; f < 1e-10 + 1.; f += .001)
		if(sfputd(fp,f) < 0)
			terror("Writing %f\n",f);

	sfseek(fp,(Sfoff_t)0,0);
	for(f = 1e-10, i = 0; f < 1e-10 + 1.; f += .001, ++i)
		if((v = sfgetd(fp)) != f)
			terror("Element=%d Input=%f, Expect=%f\n",i,v,f);
	unlink("xxx");

	return 0;
}
