#define _STDEXTERN

#include	"sfstdio.h"

#ifdef _STDIO_INIT
	FILE	_iob[_NFILE] =
	{
		_STDIO_INIT(0),
		_STDIO_INIT(1),
		_STDIO_INIT(2)
	};
#endif /* _STDIO_INIT */
