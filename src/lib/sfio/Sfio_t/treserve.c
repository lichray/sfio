#include	"sftest.h"


main()
{
	int	i, n, k;
	char	buf[1024], *s;
	char	bigbuf[1024*8];

	if(sfopen(sfstdout,"xxx","w") != sfstdout)
		terror("Opening xxx\n");

	sfsetbuf(sfstdout,NIL(char*),0);
	if(!(s = sfreserve(sfstdout,0,1)) )
		terror("Could not lock stdout\n");
	if(sfputc(sfstdout,'1') >= 0)
		terror("stdout wasn't locked\n");
	if(sfwrite(sfstdout,s,0) != 0)
		terror("stdout can't be unlocked\n");

	sfsetbuf(sfstdout,NIL(char*),sizeof(buf)/2);

	for(i = 0; i < sizeof(buf); ++i)
		buf[i] = (i%26) + 'a';

	n = 0;
	for(i = 0; i < 33; ++i)
	{	if(!(s = sfreserve(sfstdout,sizeof(buf),1)) )
			terror("Can't reserve write buffer\n");

		memcpy(s,buf,sizeof(buf));

		if(sfwrite(sfstdout,s,sizeof(buf)) != sizeof(buf) )
			terror("Writing to xxx\n");
		else	n += sizeof(buf);
	}

	sfsync(sfstdout);

	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Opening xxx\n");
	sfsetbuf(sfstdin,NIL(char*),8*sizeof(buf));
	if(sfsize(sfstdin) != n)
		terror("Wrong size for xxx\n");

	i = 0;
	for(;;)
	{	if(!(s = sfreserve(sfstdin,16*sizeof(buf),0)) )
			break;
		else	i += 16*sizeof(buf);
	}
	if(sfslen() > 0)
		i += sfslen();
	if(i != n)
		terror("Did not read data\n");

	sfseek(sfstdin,0L,0);
	sfsetbuf(sfstdin,bigbuf,sizeof(bigbuf));
	i = 0;
	for(;;)
	{	if(!(s = sfreserve(sfstdin,16*sizeof(buf),0)) )
			break;
		else	i += 16*sizeof(buf);
	}
	if(sfslen() > 0)
		i += sfslen();
	if(i != n)
		terror("Did not read data2\n");
	sfsetbuf(sfstdin,NIL(char*),-1);

	if(sfopen(sfstdout,"xxx","w") != sfstdout)
		terror("Can't open xxx to write\n");
	for(i = 0; i < 32; ++i)
	{	for(k = 0; k < sizeof(bigbuf); ++k)
			bigbuf[k] = '0' + (k+i)%10;
		if(sfwrite(sfstdout,bigbuf,sizeof(bigbuf)) != sizeof(bigbuf))
			terror("Writing to xxx\n");
	}
	sfclose(sfstdout);

	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Opening xxx to read\n");
	if(!(s = sfreserve(sfstdin,16*sizeof(bigbuf),0)) )
		terror("sfreserve failed\n");
	for(i = 0; i < 16; ++i)
	{	for(k = 0; k < sizeof(bigbuf); ++k)
			if(*s++ != ('0' + (k+i)%10))
				terror("Wrong data i=%d k=%d\n",i,k);
	}
	if(sfseek(sfstdin,-15*sizeof(bigbuf),1) != sizeof(bigbuf))
		terror("sfseek failed\n");
	if(sfread(sfstdin,bigbuf,sizeof(bigbuf)) != sizeof(bigbuf) )
		terror("sfread failed\n");
	s = bigbuf;
	for(i = 1; i < 2; ++i)
	{	for(k = 0; k < sizeof(bigbuf); ++k)
			if(*s++ != ('0' + (k+i)%10))
				terror("Wrong data2 i=%d k=%d\n",i,k);
	}
	if(!(s = sfreserve(sfstdin,16*sizeof(bigbuf),1)) && (sfset(sfstdin,0,0)&SF_MMAP) )
		terror("sfreserve failed2\n");
	sfread(sfstdin,s,0);
#ifdef MAP_TYPE
	if(sfreserve(sfstdin,0,0) != s)
		terror("Reserve pointer changed?\n");
#endif
	for(i = 2; i < 17; ++i)
	{	for(k = 0; k < sizeof(bigbuf); ++k)
			if(*s++ != ('0' + (k+i)%10))
				terror("Wrong data3 i=%d k=%d\n",i,k);
	}

	system("rm xxx");

	exit(0);
}
