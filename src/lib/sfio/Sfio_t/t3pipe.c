#include	"sftest.h"

#if __STD_C
main(int argc, char** argv)
#else
main(argc, argv)
int	argc;
char**	argv;
#endif
{
	Sfio_t*	f;
	char*	s;
	char*	mesg = "Children are great";
	int	maxlev = 3;

	if(argc > 1)
	{	int	level;

		if((level = atoi(argv[1])) < maxlev)
			system(sfprints("%s %d",argv[0],level+1));

		sfputr(sfstdout,mesg,'\n');
		exit(0);
	}

	f = sfpopen(NIL(Sfio_t*),sfprints("%s 1",argv[0]), "r");
	while(maxlev-- > 0)
		if(!(s = sfgetr(f,'\n',1)) || strcmp(s,mesg) != 0)
			terror("Didn't get right message back\n");

	exit(0);
}
