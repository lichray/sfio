#define _in_stdextern	1
#include	"sfstdio.h"

#if _do_iob || _do_sf
#if _do_iob
FILE	_iob[_NFILE] =
#else
FILE	_sf[_NFILE] =
#endif
{	_STDIO_INIT(0),
	_STDIO_INIT(1),
	_STDIO_INIT(2)
};

#else	/* Linux */
FILE	_IO_stdin_ = _STDIO_INIT(0);
FILE	_IO_stdout_ = _STDIO_INIT(1);
FILE	_IO_stderr_ = _STDIO_INIT(2);

#endif
