#include	"sfhdr.h"

/*	Seek function that knows discipline
**
**	Written by Kiem-Phong Vo (02/12/91)
*/
#if __STD_C
long sfsk(reg Sfio_t* f, reg long addr, reg int type, reg Sfdisc_t* disc)
#else
long sfsk(f,addr,type,disc)
reg Sfio_t	*f;
reg long	addr;
reg int		type;
reg Sfdisc_t	*disc;
#endif
{
	reg long	p;
	reg int		s, local, string;

	GETLOCAL(f,local);
	if(!local && !(f->mode&SF_LOCK))
		return -1;

	if(!(string = (f->flags&SF_STRING)))
		SFDISC(f,disc,seekf,local);

	for(;;)
	{
		if(string)
		{	SFSTRSIZE(f);
			if(type == 0)
				s = (int)addr;
			else if(type == 1)
				s = (int)(addr + f->here);
			else	s = (int)(addr + f->extent);
		}
		else
		{
			if(disc && disc->seekf)
				p = (*(disc->seekf))(f,addr,type,disc);
			else	p = lseek(f->file,addr,type);
			if(p >= 0)
				return p;
			s = -1;
		}

		if(local)
			SETLOCAL(f);
		switch(_sfexcept(f,SF_SEEK,s,disc))
		{
		case SF_EDISC:
		case SF_ECONT:
			if(string)
				return 0L;
			continue;
		default:
			return -1L;
		}
	}
}
