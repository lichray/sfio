#include	"sftest.h"

#if __STD_C
main(int argc, char** argv)
#else
main(argc, argv)
int     argc;
char    **argv;
#endif
{
	Sfio_t*	f;
	char*		s;

	if(argc > 1)
        {       /* coprocess only */
                while((s = sfgetr(sfstdin,'\n',0)) )
                        sfwrite(sfstdout,s,sfvalue(sfstdin));
		exit(0);
        }

	if(!(f = sfpopen(NIL(Sfio_t*),sfprints("%s -p",argv[0]),"r+")) )
		terror("Open coprocess\n");

	if(sfwrite(f,"123\n",4) != 4)
		terror("Write coprocess\n");
	if(sftell(f) != 4)
		terror("sftell1\n");

	if(!(s = sfreserve(f,4,0)) || strncmp(s,"123\n",4) != 0 )
		terror("Read coprocess\n");
	if(sftell(f) != 8)
		terror("sftell2\n");

	sfset(f,SF_SHARE,1);

	if(sfwrite(f,"123\n",4) != 4)
		terror("Write coprocess2\n");
	if(sftell(f) != 12)
		terror("sftell 3\n");

	if(!(s = sfreserve(f,4,0)) || strncmp(s,"123\n",4) != 0 )
		terror("Read coprocess2\n");
	if(sftell(f) != 16)
		terror("sftell 4\n");

	sfclose(f);

	return 0;
}
