#include	"sfhdr.h"

/*	Get the size of a stream.
**
**	Written by Kiem-Phong Vo (02/12/91)
*/
#if __STD_C
long sfsize(reg Sfio_t *f)
#else
long sfsize(f)
reg Sfio_t	*f;
#endif
{
	reg int	mode;

	if((mode = f->mode&SF_RDWR) != f->mode && _sfmode(f,mode,0) < 0)
		return -1L;

	if(f->flags&SF_STRING)
	{	SFSTRSIZE(f);
		return f->extent;
	}

	if(f->extent >= 0 && (f->flags&(SF_SHARE|SF_APPENDWR)) )
	{	reg Sfdisc_t*	disc;
		for(disc = f->disc; disc; disc = disc->disc)
			if(disc->seekf)
				break;
		if(disc)
		{	f->extent = SFSK(f,0L,2,disc);
			(void)SFSK(f,f->here,0,disc);
		}
		else
		{	struct stat	st;
			if(fstat(f->file,&st) < 0)
				f->extent = -1L;
			else	f->extent = st.st_size;
		}
	}

	if(f->extent < 0)
		return -1L;
	else if(f->mode&SF_READ)
		return f->extent;
	else if(f->flags&SF_APPENDWR)
		return f->extent + (f->next - f->data);
	else
	{	reg long s;
		if((f->flags&(SF_SHARE|SF_PUBLIC)) == (SF_SHARE|SF_PUBLIC))
		{	if((s = SFSK(f,0L,1,f->disc)) < 0)
				return -1L;
			f->here = s;
		}
		s = f->here + (f->next - f->data);
		return s >= f->extent ? s : f->extent;
	}
}
