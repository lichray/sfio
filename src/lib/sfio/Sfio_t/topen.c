#include	"sftest.h"

#if __STD_C
main(void)
#else
main()
#endif
{
	Sfio_t*	f;
	Sfio_t		sf;

	if(sfopen(sfstdout,"abc","s") != sfstdout)
		terror("Bad reopening of sfstdout\n");
	if(sfopen(sfstdin,"123","s") != sfstdin)
		terror("Bad reopening of sfstdin\n");
	sfclose(sfstdin);
	if(sfopen(sfstdin,"123","s") != sfstdin)
		terror("Bad reopening of sfstdin\n");
	if(!(f = sfopen(NIL(Sfio_t*),"123","s")) )
		terror("Opening a stream\n");
	sfclose(f);
	if(sfopen(f,"123","s") != NIL(Sfio_t*))
		terror("can't do this!\n");

	if(sfnew(&sf,NIL(char*),-1,0,SF_EOF|SF_READ) != &sf)
		terror("Did not open sf\n");
	sfset(&sf,SF_STATIC,1);
	if(sfclose(&sf) < 0 || !(sf.flags&SF_STATIC))
		terror("Did not clode sf\n");

	exit(0);
}
