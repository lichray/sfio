#include	"sftest.h"

#if __STD_C
main(void)
#else
main()
#endif
{
	Sfio_t	*f;
	int	n;
	char	*s, *os, *endos;
	char	buf[8192];

	os = "123\n456\n789\n";
	if(!(f = sfopen((Sfio_t*)0,os,"s")))
		terror("Opening string\n");

	endos = os + strlen(os);
	while((s = sfgetr(f,'\n',0)) )
	{	
		if(s != os)
			terror("Did not get string\n");
		os += sfslen();
	}

	if(os != endos)
		terror("Did not match everything\n");

	if(sfgetc(f) >= 0 || !sfeof(f))
		terror("Stream should have exhausted\n");

	if(!(f = sfopen(f,(char*)0,"s+")))
		terror("Opening string for r/w\n");
	for(n = 0; n < 26; ++n)
		if((sfputc(f,'a'+n)) != 'a'+n)
			terror("Output\n");
	if(sfgetc(f) >= 0)
		terror("Read a non-existent byte\n");
	sfseek(f,0L,0);
	if(!(s = sfreserve(f,26,0)) )
		terror("Didnot get the right amount of data\n");
	for(n = 0; n < 26; ++n)
		if((sfputc(f,'a'+n)) != 'a'+n)
			terror("Output2\n");
	sfseek(f,2L,0);
	if(!(s = sfreserve(f,50,0)) )
		terror("Didnot get the right amount of data2\n");

	if(!(f = sfopen(f,(char*)0,"s+")))
		terror("Opening string for r/w\n");
	sfset(f,SF_READ,0);
	sfseek(f,0L,0);
	if(!(s = sfreserve(f,-1,1)) || (n = sfslen()) <= 0 || sfwrite(f,s,0) != 0)
		terror("Buffer size should be positive\n");
	sfseek(f,n+8192L,0);
	sfseek(f,0L,0);
	if(!(s = sfreserve(f,-1,1)) || sfslen() != (n+8192) || sfwrite(f,s,0) != 0)
		terror("Bad buffer size\n");

	if(!(f = sfopen(f,(char*)0,"s+")))
		terror("Opening string for r/w\n");
	if(sfwrite(f,buf,sizeof(buf)) != sizeof(buf))
		terror("Can't write large buffer\n");

	if(!(f = sfopen((Sfio_t*)0,(char*)0,"s+")))
		terror("Opening string for r/w\n");
	sfset(f,SF_READ,0);
	for(n = 0; n < 16*1024; ++n)
	{
         	if((n%1024) == 0)
		{	long a = sfseek(f,1024L,1);
			sfputc(f,'a');
			sfseek(f,-1025L,1);
		}
                sfputc(f,'a');
	}
	sfseek(f,0L,0);
	if(!(s = sfreserve(f,-1,1)) || sfslen() != n+1024 || sfwrite(f,s,0) != 0)
		terror("Wrong buffer size\n");
	while(n-- > 0)
		if(*s++ != 'a')
			terror("Wrong data\n");

	if(!(f = sfopen((Sfio_t*)0,(char*)0,"s+")))
		terror("Opening r/w string\n");
	for(n = 0; n < 10; n++)
		sfputc(f,'a'+n);
	sfputc(f,'\n');
	sfseek(f,0L,0);
	for(n = 0; n <= 11 ; ++n)
		if(sfgetc(f) != 'a'+n)
			break;
	if(n != 10)
		terror("Get too many\n");
	if(sfgetc(f) >= 0)
		terror("Reading non-existent data\n");
	if(!sfeof(f))
		terror("Didn't get eof\n");

	return 0;
}
