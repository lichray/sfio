#include	"sftest.h"

main()
{
	char		buf[6000];
	ssize_t		n, p, w;
	char*		s;
	Sfio_t*	f;

	if(sfopen(sfstdout,"xxx","w") != sfstdout)
		terror("Can't open xxx to write\n");
	for(n = 0; n < 1000; ++n)
		if(sfputr(sfstdout,"012345678\n",-1) != 10)
			terror("Can't write to xxx\n");
	sfsync(sfstdout);

	if(sfopen(sfstdin,"xxx","r+") != sfstdin)
		terror("Can't open xxx to read+write\n");

	if(sfseek(sfstdin,(Sfoff_t)4,0) != 4)
		terror("Can't seek\n");
	if(sfstdin->bits&SF_MMAP)
		terror("Unexpected memory mapping\n");
	if(sfpeek(sfstdin,(Void_t**)&s,-1) <= 0)
		terror("Bad peek\n");
	if(s[0] != '4')
		terror("Bad data\n");
	s[0] = 'a';
	if(sfwrite(sfstdin,s,1) != 1)
		terror("Bad write\n");
	if(sfseek(sfstdin,(Sfoff_t)0,0) != 0)
		terror("Bad seek2\n");

	if(!(f = sfopen(NIL(Sfio_t*),"xxx","r")) )
		terror("Bad open\n");
	if(sfpeek(f,(Void_t**)&s,10) != 10)
		terror("Bad peek2\n");
	if(strncmp(s,"0123a5678\n",10) != 0)
		terror("Wrong data in xxx\n");
	sfclose(f);

	if(sfopen(sfstdout,"yyy","w") != sfstdout)
		terror("Can't open yyy to write\n");

	/* possibly using mmap */
	while((n = sfpeek(sfstdin,(Void_t**)&s,1000)) > 0)
		if((w = sfwrite(sfstdout,s,n)) != n)
			terror("Can't write to yyy\n");
	sfclose(sfstdout);
	if(system("cmp -s xxx yyy") != 0)
		terror("xxx not equal yyy first time\n");

	if(sfopen(sfstdout,"yyy","w") != sfstdout)
		terror("Can't open yyy to write2\n");

	/* use our own buffer */
	sfseek(sfstdin,(Sfoff_t)0,0);
	sfsetbuf(sfstdin,buf,sizeof(buf));
	while((n = sfpeek(sfstdin,(Void_t**)&s,5000)) > 0)
		if((w = sfwrite(sfstdout,s,n)) != n)
			terror("Can't write to yyy2\n");
	sfclose(sfstdout);
	if(system("cmp -s xxx yyy") != 0)
		terror("xxx not equal yyy second time\n");

	if(sfopen(sfstdout,"yyy","w+") != sfstdout)
		terror("Can't open yyy to write3\n");
	sfwrite(sfstdout,"abcd",4);
	sfseek(sfstdout,(Sfoff_t)1,0);
	if(sfpeek(sfstdout,(Void_t**)&s,-1) != 3)
		terror("Failure reading yyy\n");
	s[2] = 'e';
	s[3] = '\0';
	if((w = sfwrite(sfstdout,s,3)) != 3)
		terror("Failure writing yyy w=%d\n",w);
	if(sfopen(sfstdout,"yyy","r") != sfstdout)
		terror("Can't open yyy for read3\n");
	if((p = sfpeek(sfstdout,(Void_t**)&s,-1)) != 4)
		terror("Failure reading yyy3\n");
	s[4] = '\0';
	if(strcmp(s,"abce") != 0)
		terror("Bad data in yyy\n");
	sfread(sfstdout,s,4);

	system("rm xxx yyy");

	if(sfopen(sfstdout,"xxx","w") != sfstdout)
		terror("Open xxx to write\n");

#define DATA	125
	for(p = 0; p < DATA; ++p)
		buf[p] = p;

	for(n = 0; n < 100; n++)
		if((w = sfwrite(sfstdout,buf,DATA)) != DATA)
			terror("Writing to xxx\n");
	sfsync(sfstdout);
	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Open xxx to read\n");
	sfsetbuf(sfstdin,buf,1024);

	n = 0;
	while((p = sfpeek(sfstdin,(Void_t**)&s,DATA)) == DATA)
	{	n += 1;
		for(p = 0; p < DATA; ++p)
			if(s[p] != p)
				terror("Corrupted data\n");
	}
	if(n != 100 || p != 0)
		terror("Wrong number of peeks\n");
	system("rm xxx");

	return 0;
}
