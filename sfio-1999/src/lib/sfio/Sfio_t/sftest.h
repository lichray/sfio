#if _SFIO_H_ONLY
#include	<sfio.h>

_BEGIN_EXTERNS_
extern int	unlink _ARG_((const char*));
extern int	write _ARG_((int, const void*, int));
extern int	exit _ARG_((int));
extern size_t	strlen _ARG_((const char*));
extern Void_t*	malloc _ARG_((size_t));
_END_EXTERNS_

#else
#include	"sfhdr.h"
#endif

#ifndef NIL
#define NIL(t)	((t)0)
#endif

_BEGIN_EXTERNS_
extern int	strncmp _ARG_((const char*, const char*, size_t));
extern int	strcmp _ARG_((const char*, const char*));

extern int	system _ARG_((const char*));
extern int	alarm _ARG_((int));
_END_EXTERNS_

/* temp files that may be used */
static char*	Kpv[4] = { "/tmp/kpvaaa", "/tmp/kpvbbb", "/tmp/kpvccc", NIL(char*) };
static void rmkpv()
{	int	i;
	for(i = 0; Kpv[i]; ++i)
		unlink(Kpv[i]);
}

#ifdef __LINE__
static int	Line;
#define terror	Line=__LINE__,t_error
#else
#define terror	t_error
#endif

#if __STD_C
void t_error(char* form, ...)
#else
void t_error(va_alist)
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
#ifdef __LINE__
	sfprintf(f,"Line=%d: ",Line);
#endif
	sfvprintf(f,form,args);
	sfputc(f,'\n');
	sfputc(f,'\0');
	sfseek(f,(Sfoff_t)0,0);
	s = sfreserve(f,SF_UNBOUND,1);

	va_end(args);

	write(2,s,strlen(s));

	rmkpv();
	exit(-1);
}
