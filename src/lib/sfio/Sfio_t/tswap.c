#include	"sftest.h"

main()
{
	Sfio_t*	f1;
	Sfio_t* f2;
	char*	s;

	if(!(f1 = sfopen(NIL(Sfio_t*),"xxx","w+")) )
		terror("Can't open xxx\n");
	if(sfwrite(f1,"0123456789\n",11) != 11)
		terror("Can't write to xxx\n");

	sfclose(sfstdin);
	if(sfswap(f1,sfstdin) != sfstdin)
		terror("Can't swap with sfstdin\n");
	sfseek(sfstdin,(Sfoff_t)0,0);
	if(!(s = sfgetr(sfstdin,'\n',1)) )
		terror("sfgetr failed\n");
	if(strcmp(s,"0123456789") != 0)
		terror("Get wrong data\n");

	if(!(f1 = sfswap(sfstdin,NIL(Sfio_t*))) )
		terror("Failed swapping to NULL\n");
	if(!sfstack(sfstdout,f1) )
		terror("Failed stacking f1\n");

	if(!(f2 = sfopen(NIL(Sfio_t*),"xxx", "r")) )
		terror("Can't open xxx for read\n");

	if(sfswap(f1,f2) != NIL(Sfio_t*) )
		terror("sfswap should have failed\n");

	system("rm xxx >/dev/null 2>&1");
	return 0;
}
