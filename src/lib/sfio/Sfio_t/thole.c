#include	"sftest.h"

/*	Test for /dev/null and hole-preserving code */

main()
{
	Sfio_t*	null;
	Sfio_t*	f;
	char	buf[16];

	if(!(null = sfopen(NIL(Sfio_t*),"/dev/null","w")) )
		terror("Opening /dev/null");

	sfsetbuf(null,NIL(char*),(size_t)SF_UNBOUND);

	if(!SFISNULL(null) )
		terror("Not /dev/null?");

	if(!(f = sfopen(NIL(Sfio_t*),"xxx","w+")) )
		terror("Creating xxx");
	sfwrite(f,"1234",4);
	sfseek(f,(Sfoff_t)1,0);
	sfsync(f);

	sfsetfd(null,-1);
	sfsetfd(null,sffileno(f));
	sfsync(null);

	sfseek(f,(Sfoff_t)0,0);
	if(sfread(f,buf,4) != 4 || strncmp(buf,"1234",4) != 0)
		terror("Bad data");

	return 0;
}
