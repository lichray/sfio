#include	"sfhdr.h"
#include	"stdio.h"

#if __STD_C
int _stdfflush(FILE* f)
#else
int _stdfflush(f)
FILE*	f;
#endif
{
	if(!f)
		return -1;
	if(f->extent >= 0)
		(void)sfseek(f, (Sfoff_t)0, SEEK_CUR|SF_PUBLIC);
	return (sfsync(f) < 0 || sfpurge(f) < 0) ? -1 : 0;
}
