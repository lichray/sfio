#include	"sftest.h"

#if __STD_C
main(void)
#else
main()
#endif
{
	Sfio_t	*f;
	char	*ss, *s;
	int	n, i;
	char	zero[SF_BUFSIZE*2];
	char	buf[SF_BUFSIZE];

	s = "123456789\n";
	n = strlen(s);
	if(!(f = sfopen((Sfio_t*)0,"xxx","w")))
		terror("Opening file to write\n");
	for(i = 0; i < 1000; ++i)
		if(sfwrite(f,s,n) != n)
			terror("Writing data\n");

	if(!(f = sfopen(f,"xxx","r")))
		terror("Opening file to read\n");
	if(sfseek(f,0L,2) != (i*n))
		terror("Bad file length\n");
	if(sftell(f) != (i*n))
		terror("sftell\n");
	for(; i > 0; --i)
	{	sfseek(f,-i*n,2);
		if(!(ss = sfgetr(f,'\n',1)))
			terror("sfgetr\n");
		if(strncmp(ss,s,sfslen()-1) != 0)
			terror("Expect=%s\n",s);
	}

	if(!(f = sfopen(f,"xxx","w")) )
		terror("Open to write\n");
	for(n = sizeof(zero)-1; n >= 0; --n)
		zero[n] = 0;
	if(sfwrite(f,zero,sizeof(zero)) != sizeof(zero))
		terror("Writing data\n");
	if(sfseek(f,0L,2) != lseek(sffileno(f),0L,2))
		terror("seeking1\n");
	if(sfseek(f,-1L,2) != lseek(sffileno(f),-1L,2))
		terror("seeking2\n");

	if(!(f = sfopen(f,"xxx","w")))
		terror("Open to write2\n");
	for(n = 0; n < sizeof(buf); n++)
		buf[n] = n;
	for(n = 0; n < 256; n++)
		if(sfwrite(f,buf,sizeof(buf)) != sizeof(buf))
			terror("Writing data 2\n");
	if(!(f = sfopen(f,"xxx","r")))
		terror("Open to read2\n");
	if(sfgetc(f) != 0 && sfgetc(f) != 1)
		terror("Get first 2 bytes\n");

	if(sfseek(f,128*sizeof(buf),0) != 128*sizeof(buf) )
		terror("Seeking \n");
	for(n = 0; n < 128; ++n)
		if(sfread(f,buf,sizeof(buf)) != sizeof(buf))
			terror("Reading data\n");

	system("rm xxx >/dev/null 2>&1");
	return 0;
}
