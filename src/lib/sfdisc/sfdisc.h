#include	<sfio.h>

#ifdef __cplusplus
extern "C" {
#endif
extern Sfdisc_t*	sfdcnewskable _ARG_((Sfio_t*));
extern int		sfdcdelskable _ARG_((Sfdisc_t*));

extern Sfdisc_t*	sfdcnewtee _ARG_((Sfio_t*));
extern int		sfdcdeltee _ARG_((Sfdisc_t*));

extern Sfdisc_t*	sfdcnewfilter _ARG_((char*));
extern int		sfdcdelfilter _ARG_((Sfdisc_t*));

extern Sfdisc_t*	sfdcnewsubstream _ARG_((Sfio_t*, long, long));
extern int		sfdcdelsubstream _ARG_((Sfdisc_t*));

extern Sfdisc_t*	sfdcnewlzw _ARG_((void));
extern int		sfdcdellzw _ARG_((Sfdisc_t*));

extern Sfdisc_t*	sfdcnewunion _ARG_((Sfio_t**, int));
extern int		sfdcdelunion _ARG_((Sfdisc_t*));
#ifdef __cplusplus
}
#endif
