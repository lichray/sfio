#include	"sftest.h"

main()
{
	Sfio_t*	f;
	Sfio_t	sf;

	if(sfopen(sfstdout,"abc","s") != sfstdout)
		terror("Bad reopening of sfstdout\n");
	if(sfopen(sfstdin,"123","s") != sfstdin)
		terror("Bad reopening of sfstdin\n");
	sfclose(sfstdin);

	if(!(f = sfopen(NIL(Sfio_t*),"123","s")) )
		terror("Opening a stream\n");
	sfclose(f);
	if(sfopen(f,"123","s") != NIL(Sfio_t*))
		terror("can't reopen a closed stream!\n");

	if(sfnew(&sf,NIL(char*),SF_UNBOUND,0,SF_EOF|SF_READ) != &sf)
		terror("Did not open sf\n");
	sfset(&sf,SF_STATIC,1);
	if(sfclose(&sf) < 0 || !(sf.flags&SF_STATIC))
		terror("Did not close sf\n");

	/* test for exclusive opens */
	system("rm xxx");
	if(!(f = sfopen(NIL(Sfio_t*),"xxx","wx") ) )
		terror("sfopen failed\n");
	if((f = sfopen(f,"xxx","wx") ) )
		terror("sfopen should not succeed here\n");
	system("rm xxx");

	return 0;
}
