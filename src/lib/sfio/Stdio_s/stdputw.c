#include	"sfhdr.h"
#include	"stdio.h"

#if __STD_C
int _stdputw(int w, FILE* f)
#else
int _stdputw(w, f)
int	w;
FILE*	f;
#endif
{
	(void)sfwrite(f, &w, sizeof(int));
	return sferror(f);
}
