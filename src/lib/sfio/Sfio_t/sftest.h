#ifdef SFIO_H_ONLY
#include	"../sfio.h"
#if __STD_C
#include	<stddef.h>
#include	<stdarg.h>
#else
#include	<varargs.h>
#endif
#else
#include	"../sfhdr.h"
#endif

_BEGIN_EXTERNS_
extern void	exit _ARG_((int));
extern int	strncmp _ARG_((const char*, const char*, size_t));
extern int	strcmp _ARG_((const char*, const char*));
extern char*	system _ARG_((char*));
_END_EXTERNS_

#if __STD_C
void terror(char* form, ...)
#else
void terror(va_alist)
va_dcl
#endif
{
	Sfio_t*	f;
	char*	s;
	va_list	args;
#if __STD_C
	va_start(args,form);
#else
	char*	form;
	va_start(args);
	form = va_arg(args,char*);
#endif

	f = sfopen(NIL(Sfio_t*),NIL(char*),"sw");
	sfvprintf(f,form,args);
	sfputc(f,'\0');
	sfseek(f,0L,0);
	s = sfreserve(f,-1,1);

	va_end(args);

	write(2,s,strlen(s));
	exit(-1);
}
