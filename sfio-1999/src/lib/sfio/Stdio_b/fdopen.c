#include	"sfstdio.h"

/*	Opening a stream given a file desccriptor.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
FILE* fdopen(int fd, const char* mode)
#else
FILE* fdopen(fd,mode)
int	fd;
char*	mode;
#endif
{
	reg Sfio_t*	sp;
	reg FILE*	fp;
	reg int		flags = 0;

	while(1) switch(*mode++)
	{
	case 0:
		goto e_mode;
	case 'r':
		flags = SF_READ;
		break;
	case 'w':
		flags = SF_WRITE;
		break;
	case 'a':
		flags = SF_WRITE|SF_APPENDWR;
		break;
	case '+':
		flags = SF_READ|SF_WRITE;
		break;
	case 'b':
		break;
	default:
		return NIL(FILE*);
	}
e_mode :
	if(!flags)
		return NIL(FILE*);

	if(!(sp = sfnew(NIL(Sfio_t*), NIL(Void_t*), (size_t)SF_UNBOUND, fd, flags)))
		return NIL(FILE*);
	if(!(fp = _stdstream(sp)))
	{	sfclose(sp);
		return NIL(FILE*);
	}

	return(fp);
}
