#include	"sftest.h"


/*	Test multiple processes reading/writing from same file
**	descriptor.
*/
#if __STD_C
main(int argc, char** argv)
#else
main(argc,argv)
int	argc;
char**	argv;
#endif
{
	char*	s;

	if(argc > 1)
	{	if(strcmp(argv[1],"-r") == 0)	/* doing sfgetr */
		{	if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line2") != 0)
				terror("Coprocess getr did not get Line2\n");
			if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line3") != 0)
				terror("Coprocess getr did not get Line3\n");
		}
		else	/* doing sfmove */
		{	Sfio_t*	f = sfopen(NIL(Sfio_t*),NIL(char*),"swr");
			if(!f)
				terror("Can't open string stream\n");
			if(sfmove(sfstdin,f,2,'\n') != 2)
				terror("Coprocess sfmove failed\n");
			sfseek(f,0L,0);
			if(!(s = sfgetr(f,'\n',1)) || strcmp(s,"Line2") != 0)
				terror("Coprocess move did not get Line2\n");
			if(!(s = sfgetr(f,'\n',1)) || strcmp(s,"Line3") != 0)
				terror("Coprocess move did not get Line3\n");
		}
		exit(0);
	}

	if(sfopen(sfstdout,"xxx","w") != sfstdout )
		terror("Opening xxx\n");
	if(sfputr(sfstdout,"Line1",'\n') < 0 ||
	   sfputr(sfstdout,"Line2",'\n') < 0 ||
	   sfputr(sfstdout,"Line3",'\n') < 0 ||
	   sfputr(sfstdout,"Line4",'\n') < 0)
		terror("Writing data\n");
	sfopen(sfstdout,"/dev/null","w");

	/* testing coprocess calling sfgetr */
	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Opening xxx to read\n");
	if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line1") != 0)
		terror("Did not get Line1 for sfgetr\n");
	sfsync(sfstdin);
	system(sfprints("%s -r",argv[0]));
	sfseek(sfstdin,lseek(sffileno(sfstdin),0L,1),0);
	if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line4") != 0)
		terror("Did not get Line4 for sfgetr\n");

	/* testing coprocess calling sfmove */
	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Opening xxx to read\n");
	if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line1") != 0)
		terror("Did not get Line1 for sfmove\n");
	sfsync(sfstdin);
	system(sfprints("%s -m",argv[0]));
	sfseek(sfstdin,lseek(sffileno(sfstdin),0L,1),0);
	if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line4") != 0)
		terror("Did not get Line4 for sfmove\n");

	/* testing the head program */
#ifdef HEAD
	if(sfopen(sfstdin,"xxx","r") != sfstdin)
		terror("Opening xxx to read\n");
	if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line1") != 0)
		terror("Did not get Line1 for head\n");
	sfsync(sfstdin);
	system("/usr/addon/ast/bin/head -2 > /dev/null");
	sfseek(sfstdin,lseek(sffileno(sfstdin),0L,1),0);
	if(!(s = sfgetr(sfstdin,'\n',1)) || strcmp(s,"Line4") != 0)
		terror("Did not get Line4 for head\n");
#endif

	system("rm xxx");
	exit(0);
}
