#define _in_stdextern	1
#include	"sfstdio.h"

#if _do_iob
#define _have_streams	1
FILE	_iob[_NFILE] =
{	_STDIO_INIT(0),
	_STDIO_INIT(1),
	_STDIO_INIT(2)
};
#endif

#if _do_sf && !_have_streams	/* BSDI2.0 */
FILE	_sf[_NFILE] =
{	_STDIO_INIT(0),
	_STDIO_INIT(1),
	_STDIO_INIT(2)
};
#endif

#if _do_swbuf && _do_srget && !_have_streams	/*BSDI3.0*/
#define _have_streams	1
FILE	__sstdin = _STDIO_INIT(0);
FILE	__sstdout = _STDIO_INIT(1);
FILE	__sstderr = _STDIO_INIT(2);
#endif

#if _do_self_stdin && !_have_streams /*Linux Redhat 6.0*/
#define _have_streams	1
FILE	_Stdin_ = _STDIO_INIT(0),  *stdin  = &_Stdin_;
FILE	_Stdout_ = _STDIO_INIT(1), *stdout = &_Stdout_;
FILE	_Stderr_ = _STDIO_INIT(2), *stderr = &_Stderr_;
#endif

#if _do_ampersand_stdin && !_have_streams /*Linux Redhat 5.2*/
#define _have_streams	1
FILE	_IO_stdin_  = _STDIO_INIT(0);
FILE	_IO_stdout_ = _STDIO_INIT(1);
FILE	_IO_stderr_ = _STDIO_INIT(2);
#endif

#if _do_star_stdin && !_have_streams /*Linux Redhat 5.2*/
#define _have_streams	1
FILE	_Stdin_ = _STDIO_INIT(0),  *_IO_stdin  = &_Stdin_;
FILE	_Stdout_ = _STDIO_INIT(1), *_IO_stdout = &_Stdout_;
FILE	_Stderr_ = _STDIO_INIT(2), *_IO_stderr = &_Stderr_;
#endif
