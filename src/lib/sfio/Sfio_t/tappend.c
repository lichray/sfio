#include	"sftest.h"

main()
{
	Sfile_t	*f1, *f2;
	char*	s;
	long	p;

	if(!(f1 = sfopen(NIL(Sfile_t*),"xxx","w")) )
		terror("Can't open f1\n");
	if(!(f1 = sfopen(f1,"xxx","a+")) )
		terror("Can't open f1\n");

	if(!(f2 = sfopen(NIL(Sfile_t*),"xxx","a+")) )
		terror("Can't open f2\n");

	if(sfwrite(f1,"012345678\n",10) != 10 || sfsync(f1) < 0)
		terror("Writing to f1\n");
	if((p = sftell(f1)) != 10)
		terror("Bad sftell1 %ld\n",p);

	if(sfwrite(f2,"abcdefghi\n",10) != 10 || sfsync(f2) < 0)
		terror("Writing to f2\n");
	if((p = sftell(f2)) != 20)
		terror("Bad sftell2\n");

	if((p = sfseek(f1,0L,0)) != 0L)
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

	if((p = sfseek(f2,10L,0)) != 10L)
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
	exit(0);
}
