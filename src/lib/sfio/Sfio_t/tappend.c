#include	"sftest.h"

main()
{
	Sfio_t	*f1, *f2;
	char*	s;
	Sfoff_t	p;

	if(!(f1 = sfopen(NIL(Sfio_t*),"xxx","w")) )
		terror("Can't open f1\n");
	if(!(f1 = sfopen(f1,"xxx","a+")) )
		terror("Can't open f1\n");

	if(!(f2 = sfopen(NIL(Sfio_t*),"xxx","a+")) )
		terror("Can't open f2\n");

	if(sfwrite(f1,"012345678\n",10) != 10 || sfsync(f1) < 0)
		terror("Writing to f1\n");
	if((p = sftell(f1)) != 10)
		terror("Bad sftell1 %ld\n",p);

	if(sfwrite(f2,"abcdefghi\n",10) != 10 || sfsync(f2) < 0)
		terror("Writing to f2\n");
	if((p = sftell(f2)) != 20)
		terror("Bad sftell2\n");

	if((p = sfseek(f1,(Sfoff_t)0,0)) != 0)
		terror("Bad seek\n");
	if(!(s = sfgetr(f1,'\n',1)) )
		terror("Bad getr1\n");
	if(strcmp(s,"012345678") != 0)
		terror("Bad input1\n");

	if((p = sftell(f1)) != 10)
		terror("Bad sftell3\n");

	if(sfwrite(f1,"012345678\n",10) != 10 || sfsync(f1) < 0)
		terror("Writing to f1\n");
	if((p = sftell(f1)) != 30)
		terror("Bad sftell4\n");

	if((p = sfseek(f2,(Sfoff_t)10,0)) != 10)
		terror("Bad seek\n");
	if(!(s = sfgetr(f2,'\n',1)) )
		terror("Bad getr2\n");
	if(strcmp(s,"abcdefghi") != 0)
		terror("Bad input2\n");

	if(!(s = sfgetr(f2,'\n',1)) )
		terror("Bad getr3\n");
	if(strcmp(s,"012345678") != 0)
		terror("Bad input3\n");

	system("rm xxx >/dev/null 2>&1");
	return 0;
}
