#include	"sftest.h"

#if __STD_C
main(void)
#else
main()
#endif
{
	int	i, r;
	Sfio_t	*fp;

	if(!(fp = sftmp(8)))
		terror("Can't open temp file\n");

	for(i = -5448; i <= 5448; i += 101)
		if(sfputl(fp,i) < 0)
			terror("Writing %d\n",i);

	sfseek(fp,0L,0);

	for(i = -5448; i <= 5448; i += 101)
		if((r = (int)sfgetl(fp)) != i)
			terror("Input=%d, Expect=%d\n",r,i);
	return 0;
}
