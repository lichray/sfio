#define SFIO_H_ONLY	1
#include	"sftest.h"

/* This test causes mmap() to fail so that read() must be used. */

#if __STD_C
void* mmap(void* addr, size_t size, int x, int y, int z, long offset)
#else
void* mmap()
#endif
{
	return (void*)(-1);
}

main()
{
	Sfio_t*	f;
	char	buf[1024], buf2[1024];
	int	n, r;

	if(!(f = sfopen(NIL(Sfio_t*),"xxx","w")) )
		terror("Can't open xxx to write\n");

	for(n = 0; n < sizeof(buf); ++n)
		buf[n] = '0' + (n%10);

	for(n = 0; n < 10; ++n)
		sfwrite(f,buf,sizeof(buf));

	if(!(f = sfopen(f,"xxx","r")) )
		terror("Can't open xxx to read\n");

	for(n = 0; n < 10; ++n)
	{	if((r = sfread(f,buf2,sizeof(buf))) != sizeof(buf))
			terror("Bad read size=%d\n",r);
		if(strncmp(buf,buf2,sizeof(buf)) != 0)
			terror("Get wrong data\n");
	}

	exit(0);
}
