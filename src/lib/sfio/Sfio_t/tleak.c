#include	"sftest.h"

main()
{
	if(sfopen(sfstdout,"xxx","w") != sfstdout)
		terror("Can't open xxx to write\n");
	if(sfputr(sfstdout,"012345678\n",-1) != 10)
		terror("Can't write to xxx\n");
	sfsync(sfstdout);

	if(sfopen(sfstdout,"yyy","w") != sfstdout)
		terror("Can't open yyy to write\n");
	sfclose(sfstdout);

	return 0;
}
