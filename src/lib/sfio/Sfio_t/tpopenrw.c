#include	"sftest.h"

#if __STD_C
main(int argc, char** argv)
#else
main(argc, argv)
int	argc;
char	**argv;
#endif
{
	Sfio_t	*f;
	char	buf[1024], *s;
	int	n;

	if(argc > 1)
	{	/* coprocess only */
		while(s = sfreserve(sfstdin,-1,0))
			sfwrite(sfstdout,s,sfslen());
		return 0;
	}

	/* make coprocess */
	if(!(f = sfpopen(NIL(Sfio_t*), sfprints("%s -p",argv[0]), "r+")))
		terror("Opening for read/write\n");
	for(n = 0; n < 10; ++n)
	{	sfsprintf(buf,sizeof(buf),"Line %d",n);
		sfputr(f,buf,'\n');
		if(!(s = sfgetr(f,'\n',1)))
			terror("Did not read back line\n");
		if(strcmp(s,buf) != 0)
			terror("Input=%s, Expect=%s\n",s,buf);
	}

	sfputr(f,"123456789",'\n');
	sfsync(f);
	sleep(1);
	if(!(s = sfreserve(f,-1,1)) || sfslen() != 10)
		terror("Did not get data back\n");
	if(strncmp(s,"123456789\n",10) != 0)
		terror("Wrong data\n");
	s[0] = s[1] = s[2] = '0';
	if(sfwrite(f,s,3) != 3 || sfputc(f,'\n') != '\n')
		terror("Fail on write\n");
	if(!(s = sfgetr(f,'\n',1)) )
		terror("Lost data\n"); 
	if(strcmp(s,"456789") != 0)
		terror("Wrong data2\n");
	if(!(s = sfgetr(f,'\n',1)) )
		terror("Lost data2\n"); 
	if(strcmp(s,"000") != 0)
		terror("Wrong data3\n");
	
	return 0;
}
