#include	"sftest.h"

#if __STD_C
main(void)
#else
main()
#endif
{
	Sfio_t	*f;

	if(!(f = sfopen((Sfio_t*)0,"xxx","w")))
		terror("Opening xxx to write\n");
	if(sfputc(f,'a') != 'a')
		terror("sfputc\n");
	if(sfgetc(f) >= 0)
		terror("sfgetc\n");
	
	if(!(f = sfopen(f,"xxx","r")))
		terror("Opening xxx to read\n");
	if(sfgetc(f) != 'a')
		terror("sfgetc2\n");
	if(sfputc(f,'b') >= 0)
		terror("sfputc2\n");

	if(!(f = sfopen(f,"xxx","r+")))
		terror("Opening xxx to read/write\n");

	if(sfgetc(f) != 'a')
		terror("sfgetc3\n");
	if(sfputc(f,'b') != 'b')
		terror("sfputc3\n");
	if(sfclose(f) < 0)
		terror("sfclose\n");

	if(!(f = sfpopen(NIL(Sfio_t*),"cat xxx","r")))
		terror("sfpopen\n");
	if(sfgetc(f) != 'a')
		terror("sfgetc4\n");
	if(sfgetc(f) != 'b')
		terror("sfgetc5\n");
	if(sfgetc(f) >= 0)
		terror("sfgetc6\n");

	if(!(f = sfopen(f,"xxx","w")) )
		terror("sfopen\n");
	if(sfputc(f,'a') != 'a')
		terror("sfputc1\n");
	sfsetfd(f,-1);
	if(sfputc(f,'b') >= 0)
		terror("sfputc2\n");
	if(sfclose(f) < 0)
		terror("sfclose\n");

	return 0;
}
