#include	"sftest.h"

main()
{
	Sfio_t	*f, *f2;
	char*	s;
	char	buf[20];

	if(!(f = sfopen(NIL(Sfio_t*),"xxx", "w+") ) )
		terror("Can't open file\n");

	if(sfnputc(f,'a',1000) != 1000)
		terror("Writing\n");

	if(sfseek(f,0L,0) != 0L)
		terror("Seeking\n");

	if(sfsize(f) != 1000L)
		terror("Wrong size\n");

	if(!(f2 = sfnew(NIL(Sfio_t*),NIL(char*),-1,sffileno(f),SF_WRITE)) )
		terror("Can't open stream\n");

	if(sfseek(f2,1999L,0) != 1999L)
		terror("Seeking2\n");
	sfputc(f2,'b');
	sfsync(f2);
	if(sfsize(f2) != 2000L)
		terror("Wrong size 2\n");

	if(sfsize(f) != 1000L)
		terror("Wrong size 3\n");

	sfputc(f,'a');
	sfset(f,SF_SHARE,1);
	if(sfsize(f) != 2000L)
		terror("Wrong size 4\n");

	if(!(f = sfopen(f,NIL(char*),"srw")) )
		terror("Can't open string stream\n");

	sfwrite(f,"0123456789",10);
	if(sfsize(f) != 10)
		terror("String size is wrong1\n");
	sfseek(f,19L,0);
	if(sfsize(f) != 10)
		terror("String size is wrong2\n");
	sfputc(f,'a');
	if(sfsize(f) != 20)
		terror("String size is wrong3\n");
	sfseek(f,0L,0);
	if(sfsize(f) != 20)
		terror("String size is wrong4\n");
	sfseek(f,0L,0);
	if(!(s = sfreserve(f,-1,1)) && sfslen() != 20)
		terror("String size is wrong5\n");
	sfread(f,s,5);
	if(sfsize(f) != 20)
		terror("String size is wrong6\n");
	sfwrite(f,"01234567890123456789",20);
	if(sfsize(f) != 25)
		terror("String size is wrong7\n");

	strcpy(buf,"0123456789");
	if(!(f = sfopen(f,buf,"s+")) )
		terror("Can't open string stream2\n");
	if(sfset(f,0,0)&SF_MALLOC)
		terror("SF_MALLOC should not have been set\n");
	if(sfsize(f) != 10)
		terror("String size is wrong8\n");
	sfread(f,buf,5);
	if(sfwrite(f,"0123456789",10) != 5)
		terror("Write too much\n");
	if(sfsize(f) != 10)
		terror("String size is wrong9\n");

	system("rm xxx >/dev/null 2>&1");
	exit(0);
}
