#include	"sftest.h"

main()
{
	Sfoff_t	m;

	if(sfopen(sfstdout, Kpv[0], "w") != sfstdout)
		terror("Can't open %s to write\n", Kpv[0]);
	if(sfputr(sfstdout,"012345678\n",-1) != 10)
		terror("Can't write to %s\n", Kpv[0]);

	if(sfopen(sfstdout, Kpv[1],"w") != sfstdout)
		terror("Can't open %s to write\n", Kpv[1]);

	if(sfopen(sfstdin, Kpv[0], "r") != sfstdin)
		terror("Can't open %s to read\n", Kpv[0]);

	if((m = sfmove(sfstdin,sfstdout, (Sfoff_t)SF_UNBOUND, -1)) != 10)
		terror("Can't move data from %s to %s\n", Kpv[0], Kpv[1]);

	rmkpv();
	return 0;
}
