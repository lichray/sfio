#include	"sftest.h"

main()
{
	Sfio_t*	f;
	Sfio_t*	g;
	char*	s;
	char	buf[1024];
	int	n, i;

	if(!(f = sfopen(NIL(Sfio_t*),"xxx","w")) )
		terror("Can't open file to write\n");
	sfputr(f,"1111",'\n');
	sfputr(f,"2222",'\n');
	sfputr(f,"3333",'\n');
	sfputr(f,"4444",'\n');

	if(!(f = sfopen(f,"xxx","r")) )
		terror("Can't open file to read1\n");
	if(!(g = sfnew(NIL(Sfio_t*),NIL(char*),-1,sffileno(f),SF_READ)) )
		terror("Can't open file to read2\n");

	sfset(f,SF_SHARE|SF_PUBLIC,1);
	sfset(g,SF_SHARE|SF_PUBLIC,1);

	if(!(s = sfgetr(f,'\n',1)) || strcmp(s,"1111") != 0)
		terror("Wrong data1\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"2222") != 0)
		terror("Wrong data2\n");
	sfsync(g);
	if(!(s = sfgetr(f,'\n',1)) || strcmp(s,"3333") != 0)
		terror("Wrong data3\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"4444") != 0)
		terror("Wrong data4\n");
	sfsync(g);

	sfclose(f);
	sfclose(g);
	if(!(f = sfopen(NIL(Sfio_t*),"xxx","r+")) )
		terror("Can't open file to write2\n");
	if(!(g = sfnew(NIL(Sfio_t*),NIL(char*),-1,sffileno(f),SF_READ)) )
		terror("Can't open file to read3\n");

	sfset(f,SF_SHARE|SF_PUBLIC,1);
	sfset(g,SF_SHARE|SF_PUBLIC,1);

	if(sfputr(f,"1111",'\n') <= 0)
		terror("bad write1\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"2222") != 0)
		terror("Wrong data5\n");
	sfsync(g);
	if(sfputr(f,"3333",'\n') <= 0)
		terror("bad write2\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"4444") != 0)
		terror("Wrong data6\n");
	sfsync(g);

	if(!(f = sfopen(f,"xxx","w")) )
		terror("Can't open file to write3\n");

	for(i = 0; i < sizeof(buf); ++i)
		buf[i] = 0;
	for(i = 0; i < 256; ++i)
		if(sfwrite(f,buf,sizeof(buf)) != sizeof(buf))
			terror("Writing buffer0\n");

	for(i = 0; i < sizeof(buf); ++i)
		buf[i] = 1;
	for(i = 0; i < 256; ++i)
		if(sfwrite(f,buf,sizeof(buf)) != sizeof(buf))
			terror("Writing buffer1\n");

	if(!(f = sfopen(f,"xxx","r")) )
		terror("Can't open file to read3\n");
	sfset(f,SF_SHARE|SF_PUBLIC,1);

	for(n = 0; n < 256; ++n)
	{	if(!(s = sfreserve(f,sizeof(buf),0)) )
			terror("Can't reserve buffer1\n");
		for(i = 0; i < sizeof(buf); ++i)
			if(s[i] != 0)
				terror("Bad data1\n");
	}

	for(n = 0; n < 256; ++n)
	{	if(!(s = sfreserve(f,sizeof(buf),0)) )
			terror("Can't reserve buffer2\n");
		for(i = 0; i < sizeof(buf); ++i)
			if(s[i] != 1)
				terror("Bad data2\n");
	}

	if((s = sfreserve(f,1,0)) )
		terror("Reading beyond eof\n");

	if(!(f = sfopen(f,"xxx","w")) )
		terror("Can't open xxx to write\n");
	if(sfwrite(f,"aaa\nbbb\nccc\n",12) != 12)
		terror("Can't write to xxx\n");
	sfclose(f);
	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Can't open xxx as sfstdin\n");
	if((n = (int)sfmove(sfstdin,NIL(Sfio_t*),-1L,'\n')) != 3)
		terror("sfmove wrong number of lines %d\n",n);
	if((sfset(sfstdin,0,0)&(SF_SHARE|SF_PUBLIC)) != (SF_SHARE|SF_PUBLIC))
		terror("share&public flags are not on\n");
	if(sfseek(sfstdin,0L,0) != 0L)
		terror("Can't seek back to 0\n");
	if((n = (int)sfmove(sfstdin,NIL(Sfio_t*),2L,'\n')) != 2)
		terror("sfmove2 wrong number of lines %d\n",n);
	if((n = (int)sfmove(sfstdin,NIL(Sfio_t*),-1L,'\n')) != 1)
		terror("sfmove3 wrong number of lines %d\n",n);

	system("rm xxx >/dev/null 2>&1");
	exit(0);
}