#include	"sfhdr.h"

/*	The engine for formatting data
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#define HIGHBITI	(~((~((uint)0)) >> 1))
#define HIGHBITL	(~((~((Sfulong_t)0)) >> 1))

#define F_FLOAT		0000010	/* %fFeEgG format			*/
#define F_GFORMAT	0000020	/* %gG format				*/

#define F_LEFT		0000100	/* left justification (-)		*/
#define F_SIGN		0000200	/* must set a sign - or +		*/
#define F_BLANK		0000400	/* if not - and +, then prepend a blank */
#define F_ZERO		0001000	/* zero padding on the left side	*/
#define F_ALTER		0002000	/* various alternative formats (#)	*/
#define F_PAD		0004000	/* there will be some sort of padding	*/

#define F_REPEAT	0010000	/* repeat pattern up to precision	*/
#define F_MINUS		0020000	/* has a minus sign			*/
#define F_PREFIX	(F_MINUS|F_SIGN|F_BLANK)

#define FPRECIS		6	/* default precision for floats 	*/

#define GETARG(f,t, elt,elt_t,arg_t, args,fe,rv, fmt, flg,n_flg, bs,pr, typ,n_typ) \
	{ if(fe && fe->argf) \
	  { FMTPARAM(fe, fmt, flg,n_flg, bs,pr, typ,n_typ); \
	    if((t = (*fe->argf)(f,(Void_t*)(&rv),fe) ) <= 0 ) \
	    { if(t == 0)	elt = (elt_t)va_arg(args,arg_t); \
	      else		goto pop_fmt; \
	    } else		elt = (elt_t)rv; \
	  } else		elt = (elt_t)va_arg(args,arg_t); \
	}

#ifndef KPVDEL	/* emulation of %:, %& and %@ - to be removed on next release */
typedef int(*Oldarg_f)_ARG_((int,char*,char*,int));
typedef int(*Oldext_f)_ARG_((char*,int,int,char**,int,char*,int));
typedef struct _oldfmt_s
{	Sffmt_t		fe;	/* format environment	*/
	Oldarg_f	argf;	/* old argf function	*/
	Oldext_f	extf;	/* old extf function	*/
	va_listarg	args;	/* old arg list		*/
} Oldfmt_t;

#if __STD_C
static int oldarg(Sfio_t* f, Void_t* v, Sffmt_t* fe)
#else
static int oldarg(f, v, fe)
Sfio_t* 	f;
Void_t*		v;
Sffmt_t*	fe;
#endif
{	Oldfmt_t*	ofe = (Oldfmt_t*)fe;
	reg int		fmt;

	fmt = fe->fmt;
	if(fmt == 'd' || fmt == 'i')
		fmt = fe->flag == 'l' ? 'D' : 'd';
	else if(fmt == 'f' || fmt == 'e' || fmt == 'E' || fmt == 'g' || fmt == 'G')
		fmt = fe->flag == 'l' ? 'F' : 'f';
	else if(fmt == 'o' || fmt == 'x' || fmt == 'X' || fmt == 'u')
		fmt = fe->flag == 'l' ? 'U' : 'u';
	else if(fmt == 'n')
		fmt = fe->flag == 'l' ? 'N' : 'n';
	else if(fmt == 's')
		fmt = fe->base >= 0 ? 'S' : 's';
	else if(fmt == 'c')
		fmt = fe->base >= 0 ? 's' : 'c';
	else if(fmt == '*')
		fmt = 'd';

	return (*ofe->argf)(fmt,v,fe->t_str,fe->n_str) < 0 ? -1 : 1;
}
#if __STD_C
static int oldext(Sfio_t* f, Void_t* v, int width, Sffmt_t* fe)
#else
static int oldext(f, v, width, fe)
Sfio_t*		f;
Void_t*		v;
int		width;
Sffmt_t*	fe;
#endif
{	Oldfmt_t*	ofe = (Oldfmt_t*)fe;
	char*		str = NIL(char*);
	int		rv;

	rv = (*ofe->extf)(v, fe->fmt, fe->precis, &str, fe->base, fe->t_str,fe->n_str);
	if(str)
	{	fe->t_str = str;
		fe->n_str = rv;
		return 1;
	}
	else	return rv < 0 ? -1 : 0;
}
#endif/*KPVDEL*/

#if __STD_C
int sfvprintf(Sfio_t* f, const char* form, va_list args)
#else
int sfvprintf(f,form,args)
Sfio_t*	f;		/* file to print to	*/
char*	form;		/* format to use	*/
va_list	args;		/* arg list if !argf	*/
#endif
{
	reg int		n, v, n_s, base, fmt, flags;
	reg char	*sp, *ssp, *d;
	reg Sflong_t	lv;
	char		*ep, *endep, *endsp, *endd;
	int		dot, width, precis, n_output;
	int		sign, decpt, n_lhL, t;
	Sfdouble_t	dval;		/* double or long double	*/
	Argrv_t		argrv;		/* for argf to return value	*/
	Sffmt_t		*ft, *ftenv;	/* format environment		*/
	Fmt_t		*fm, *fmstk;	/* stack contexts		*/
	char*		t_user;		/* stuff between ()		*/
	int		n_user;		/* its length			*/
	char		*tlist[2], **list;
	char		buf[SF_MAXDIGITS], data[SF_GRAIN];
#if _lib_locale
	int		dc = 0;
	struct lconv*	locv;
#endif
#ifndef KPVDEL
	Oldfmt_t	ofe;
	ofe.argf = NIL(Oldarg_f); ofe.extf = NIL(Oldext_f);
	ofe.fe.argf = NIL(Sfarg_f); ofe.fe.extf = NIL(Sfext_f);
	ofe.fe.form = NIL(char*);
#endif

	/* fast io system */
#define SFBUF(f)	(d = (char*)f->next, endd = (char*)f->endb)
#define SFINIT(f)	(SFBUF(f), n_output = 0)
#define SFEND(f)	((n_output += (uchar*)d - f->next), (f->next = (uchar*)d))
#define SFputc(f,c) \
	{ if(d >= endd) \
		{ SFEND(f); if(SFFLSBUF(f,c) <  0) goto done; n_output += 1; SFBUF(f); }\
	  else	{ *d++ = (char)c; } \
	}
#define SFnputc(f,c,n) \
	{ if((endd-d) < n) \
		{ SFEND(f); if(SFNPUTC(f,c,n) != n) goto done; \
		  n_output += n; SFBUF(f); } \
	  else	{ while(n--) *d++ = (char)c; } \
	}
#define SFwrite(f,s,n) \
	{ if((endd-d) < n) \
		{ SFEND(f); if(SFWRITE(f,(Void_t*)s,n) != n) goto done; \
		  n_output += n; SFBUF(f); } \
	  else	MEMCPY(d,s,n); \
	}

	/* make sure stream is in write mode and buffer is not NULL */
	if(f->mode != SF_WRITE && _sfmode(f,SF_WRITE,0) < 0)
		return -1;

	SFLOCK(f,0);

	if(!f->data && !(f->flags&SF_STRING))
	{	f->data = f->next = (uchar*)data;
		f->endb = f->data+sizeof(data);
	}
	SFINIT(f);

	tlist[1] = NIL(char*);
	fmstk = NIL(Fmt_t*);
	ftenv = NIL(Sffmt_t*);

loop_fmt :
	while((n = *form++) )
	{
		flags = base = v = 0;
		width = precis = n_s = -1;
		endep = ep = NIL(char*);

		if(n != '%') /* collect the non-pattern chars */
		{	sp = ssp = (char*)(form-1);
			while((n = *++ssp) && n != '%')
				;
			form = endsp = ssp;
			goto do_output;
		}

		ssp = _Sfdigits;
		endsp = sp = buf+(sizeof(buf)-1);
		t_user = NIL(char*);
		n_user = dot = n_lhL = 0;

	loop_flags:	/* LOOP FOR FLAGS, WIDTH, PRECISION, BASE, TYPE */
#define LEFTP	'('
#define RIGHTP	')'
		switch((fmt = *form++) )
		{
		case LEFTP : /* get the type which is enclosed in balanced () */
			t_user = (char*)form;
			for(v = 1;;)
			{	switch(*form++)
				{
				case 0 :	/* not balancable, retract */
					form = t_user;
					t_user = NIL(char*);
					n_user = 0;
					goto loop_flags;
				case LEFTP :	/* increasing nested level */
					v += 1;
					continue;
				case RIGHTP :	/* decreasing nested level */
					if((v -= 1) != 0)
						continue;
					n_user = (form-1) - t_user;
					goto loop_flags;
				}
			}

		case '-' :
			flags |= F_LEFT;
			goto loop_flags;
		case ' ' :
			flags |= F_BLANK;
			goto loop_flags;
		case '+' :
			flags |= F_SIGN;
			goto loop_flags;
		case '#' :
			flags |= F_ALTER;
			goto loop_flags;
		case '.' :	/* argument count */
			if((dot += 1) > 2)
			{	form -= 1;
				continue;
			}
			else if(dot == 2)
				base = 0;
			goto loop_flags;
		case '*' :	/* variable width, precision, or base */
			if((dot == 0 && width >= 0) || (dot == 1 && precis >= 0) )
			{	form -= 1;	/* bad pattern specification */
				continue;
			}
			GETARG(f,t, v,int,int, args,ftenv,argrv.i,
				'*', 0,dot, 0,0, NIL(char*),0 );
			goto set_args;
		case '0' :	/* defining width or precision */
			if(dot == 0)
			{	flags |= F_ZERO;
				goto loop_flags;
			}
			/* fall thru */
		case '1' : case '2' : case '3' :
		case '4' : case '5' : case '6' :
		case '7' : case '8' : case '9' :
			v = fmt - '0';
			for(n = *form; isdigit(n); n = *++form)
				v = (v<<3) + (v<<1) + (n - '0');
		set_args:
			if(dot == 0)
			{	if((width = v) < 0)
				{	width = -width;
					flags |= F_LEFT;
				}
				flags |= F_PAD;
			}
			else if(dot == 1)
				precis = v;
			else	base = v;
			goto loop_flags;

			/* modifiers for object's length */
		case 'l' :
			flags = (flags&~(F_SHORT|F_LDOUBLE))|F_LONG;
			n_lhL += 1;
			goto loop_flags;
		case 'h' :
			flags = (flags&~(F_LONG|F_LDOUBLE))|F_SHORT;
			n_lhL += 1;
			goto loop_flags;
		case 'L' :
			flags = (flags&~(F_LONG|F_SHORT))|F_LDOUBLE;
			n_lhL += 1;
			goto loop_flags;

			/* PRINTF DIRECTIVES */
		default :	/* unknown directive */
			if(ftenv && ftenv->extf)
			{	va_list	savarg; va_copy(savarg, args);

				GETARG(f,t, argrv.vp,Void_t*,Void_t*, args,ftenv,argrv.vp,
					fmt, flags,n_lhL, base,precis, t_user,n_user );

				FMTPARAM(ftenv,fmt,flags,n_lhL,base,precis,t_user,n_user);
				if((t = (*ftenv->extf)(f, argrv.vp, width, ftenv)) > 0)
				{	sp = ftenv->t_str;
					n_s = ftenv->n_str;
					goto s_format;
				}
				else if(t < 0)
					goto pop_fmt;
				else	va_copy(args, savarg);
			}

			form -= 1;
			continue;
#ifndef KPVDEL
		case '@' :
			ofe.argf = va_arg(args,Oldarg_f);
			ofe.fe.argf = oldarg;
			goto set_stack;
		case '&' :
			ofe.extf = va_arg(args,Oldext_f);
			ofe.fe.extf = oldext;
			goto set_stack;
		case ':' :
			ofe.fe.form = va_arg(args,char*);
			ofe.args = va_arg(args,va_listarg);
			va_copy(ofe.fe.args,va_listval(ofe.args));
		set_stack:
			if(!(fm = fmstk) || ftenv != &ofe.fe )
			{	if(!FMTALLOC(fm))
					goto done;
				fm->form = NIL(char*);
			}
			if(fmt == ':')
			{	fm->form = (char*)form;
				va_copy(fm->args,args);
				form = ofe.fe.form;
				va_copy(args,ofe.fe.args);
			}
			if(fm != fmstk)
			{	fm->ftenv = ftenv;
				fm->next = fmstk;
				fmstk = fm;
			}
			ftenv = &ofe.fe;
			continue;
#endif/*KPVDEL*/
		case '!' :	/* stacking a new environment */
			GETARG(f,t, ft,Sffmt_t*,Sffmt_t*, args,ftenv,argrv.ft,
				'!', flags,n_lhL, 0,0, t_user,n_user);
			if(!ft)
				goto pop_fmt;
			if(ft->form || !(fm = fmstk) )
			{	/* stack a new environment */
				if(!FMTALLOC(fm))
					goto done;

				if(ft->form)
				{	fm->form = (char*)form;
					va_copy(fm->args,args);
					form = ft->form;	
					va_copy(args,ft->args);
				}
				else	fm->form = NIL(char*);

				fm->ftenv = ftenv;
				fm->next = fmstk;
				fmstk = fm;
			}

			ftenv = ft;
			continue;

		case 's':
			if(dot == 2)	/* list of strings */
			{	GETARG(f,t, list,char**,char**, args,ftenv,list,
					's', flags,n_lhL, base,precis, t_user,n_user);
				if(!list || !list[0])
					continue;
			}
			else
			{	GETARG(f,t, sp,char*,char*, args,ftenv,argrv.s,
					's', flags,n_lhL, -1,precis, t_user,n_user);
				if(!sp)
					sp = "(null)";
			s_format:
				list = tlist; tlist[0] = sp;
			}
			for(sp = *list; ; )
			{	if((v = n_s) >= 0) /* extf says this is the length */
				{	if(precis >= 0 && v > precis)
						v = precis;
				}
				else /* precis >= 0 means min(strlen,precis) */
				{	endsp = sp;
					if((n = precis) >= 0)
						while(n-- > 0 && *endsp)
							endsp++;
					else	while(*endsp)
							endsp++;
					v = endsp - sp;
				}
				if((n = width - v) > 0)
				{	if(flags&F_ZERO)
						{ SFnputc(f,'0',n); }
					else if(!(flags&F_LEFT) )
						{ SFnputc(f,' ',n); }
				}
				SFwrite(f,sp,v);
				if(n > 0)
					{ SFnputc(f,' ',n); }
				if(!(sp = *++list))
					break;
				else if(dot == 2 && base)
					{ SFputc(f,base); }
			}
			continue;

		case '%':
			goto c_format;
		case 'c':	/* an array of characters */
			if(dot == 2)
			{	GETARG(f,t, sp,char*,char*, args,ftenv,argrv.s,
					'c', flags,n_lhL, base,precis, t_user,n_user);
				if(!sp || !sp[0])
					continue;
			}
			else
			{	GETARG(f,t, fmt,int,int, args,ftenv,argrv.c,
					'c', flags,n_lhL, -1,precis, t_user,n_user);
			c_format:
				sp = buf; buf[0] = fmt; buf[1] = 0;
			}
			if(precis <= 0)
				precis = 1;
			for(fmt = *sp;; )
			{	if((n = width-precis) > 0 && !(flags&F_LEFT))
					{ SFnputc(f,' ',n) };
				v = precis;
				SFnputc(f,fmt,v);
				if(n > 0)
					{ SFnputc(f,' ',n) };
				if(!(fmt = *++sp))
					break;
				else if(dot == 2 && base)
					{ SFputc(f,base); }
			}
			continue;

		case 'n':	/* return current output length */
			SFEND(f);
			if(!(flags&(F_LONG|F_SHORT)) )
			{	GETARG(f,t, argrv.ip,int*,int*, args,ftenv,argrv.ip,
					'n', n_lhL,flags, 0,0, t_user,n_user );
				*argrv.ip = (int)n_output;
			}
			else if(flags&F_SHORT)
			{	GETARG(f,t, argrv.hp,short*,short*, args,ftenv,argrv.hp,
					'n', n_lhL,flags, 0,0, t_user,n_user );
				*argrv.hp = (short)n_output;
			}
#if _typ_long_long
			else if(n_lhL >= 2)
			{	GETARG(f,t, argrv.llp,Sflong_t*,Sflong_t*,
					args,ftenv,argrv.llp,
					'n', n_lhL,flags, 0,0, t_user,n_user );
				*argrv.llp = (Sflong_t)n_output;
			}
#endif
			else
			{	GETARG(f,t, argrv.lp,long*,long*, args,ftenv,argrv.lp,
					'n', n_lhL,flags, 0,0, t_user,n_user );
				*argrv.lp = (long)n_output;
			}
			continue;

		case 'p':	/* pointer value */
			GETARG(f,t, argrv.vp,Void_t*,Void_t*, args,ftenv,argrv.vp,
				'p', flags,n_lhL, base,precis, t_user,n_user );
			fmt = 'x';
			base = 16; n_s = 15; n = 4;
			flags = (flags&~(F_SIGN|F_BLANK|F_ZERO))|F_ALTER;
#if _more_void_int
			lv = (Sflong_t)((Sfulong_t)argrv.vp);
			goto long_cvt;
#else
			v = (int)((uint)argrv.vp);
			goto int_cvt;
#endif
		case 'o':
			base = 8; n_s = 7; n = 3;
			flags &= ~(F_SIGN|F_BLANK);
			goto int_arg;
		case 'X':
			ssp = "0123456789ABCDEF";
		case 'x':
			base = 16; n_s = 15; n = 4;
			flags &= ~(F_SIGN|F_BLANK);
			goto int_arg;
		case 'i':
			fmt = 'd';
			goto d_format;
		case 'u':
			flags &= ~(F_SIGN|F_BLANK);
		case 'd':
		d_format:
			if(base && base >= 2 && base <= SF_RADIX)
			{	if((base&(n_s = base-1)) == 0)
				{	if(base < 8)
						n = base <  4 ? 1 : 2;
					else if(base < 32)
						n = base < 16 ? 3 : 4;
					else	n = base < 64 ? 5 : 6;
				}
				else	n_s = 0;
			}

		int_arg:
			if(!(flags&(F_SHORT|F_LONG)) )
			{	GETARG(f,t, v,int,int, args,ftenv,argrv.i,
					fmt, flags,n_lhL, base,precis, t_user,n_user );
			int_cvt:
				if(v == 0 && precis == 0)
					break;
				if(v < 0 && fmt == 'd' )
				{	flags |= F_MINUS;
					if(v == HIGHBITI) /* avoid overflow */
					{	if(!base)
							base = 10;
						v = (int)(HIGHBITI/base);
						*--sp = _Sfdigits[HIGHBITI -
							  ((uint)v)*base];
					}
					else	v = -v;
				}
				if(n_s > 0)
				{	do
					{	*--sp = ssp[v&n_s];
					} while((v = ((uint)v) >> n) );
				}
				else if(n_s != 0)
				{	sfucvt(v,sp,n,ssp,int,uint);
				}
				else
				{	do
					{	*--sp = ssp[((uint)v)%base];
					} while((v = ((uint)v)/base) );
				}
			}
			else if(flags&F_SHORT)
			{	GETARG(f,t, argrv.h,short,int, args,ftenv,argrv.h,
					fmt, flags,n_lhL, base,precis, t_user,n_user);
				v = fmt == 'd' ? (int)argrv.h : (int)((ushort)argrv.h);
				goto int_cvt;
			}
			else /*if(flags&F_LONG)*/
			{
#if _typ_long_long
				if(n_lhL >= 2)
				{	GETARG(f,t, lv,Sflong_t,Sflong_t,
					       args,ftenv,argrv.ll,
					       fmt,flags,n_lhL,base,precis,t_user,n_user);
				} else
#endif
				{
#if _more_long_int
					GETARG(f,t, argrv.l,long,long,args,ftenv,argrv.l,
					       fmt,flags,n_lhL,base,precis,t_user,n_user);
					lv = (fmt == 'd') ? (Sflong_t)argrv.l :
							    (Sflong_t)((ulong)argrv.l);
#else
					GETARG(f,t, v,int,int, args,ftenv,argrv.i,
					       fmt,flags,n_lhL,base,precis,t_user,n_user);
					goto int_cvt;
#endif /*_more_long_int*/
				}

#if _typ_long_long || _more_long_int || _more_void_int
#	if _more_void_int
			long_cvt:
#	endif
				if(lv == 0 && precis == 0)
					break;
				if(lv < 0 && fmt == 'd' )
				{	flags |= F_MINUS;
					if(lv == HIGHBITL) /* avoid overflow */
					{	if(!base)
							base = 10;
						lv = (Sflong_t)(HIGHBITL/base);
						*--sp = _Sfdigits[HIGHBITL -
							  ((Sfulong_t)lv)*base];
					}
					else	lv = -lv;
				}
				if(n_s < 0)	/* base 10 */
				{	reg Sflong_t	nv;
					sfucvt(lv,sp,nv,ssp,Sflong_t,Sfulong_t);
				}
				else if(n_s > 0) /* base power-of-2 */
				{	do
					{	*--sp = ssp[lv&n_s];
					} while((lv = ((Sfulong_t)lv) >> n) );
				}
				else		/* general base */
				{	do
					{	*--sp = ssp[((Sfulong_t)lv)%base];
					} while((lv = ((Sfulong_t)lv)/base) );
				}
#endif /* _typ_long_long || _more_long_int || _more_void_int */
			}

			/* zero padding for precision */
			for(precis -= (endsp-sp); precis > 0; --precis)
				*--sp = '0';

			if(flags&F_ALTER) /* prefix */
			{	if(fmt == 'o')
				{	if(*sp != '0')
						*--sp = '0';
				}
				else
				{	if(dot == 2 && n_s < 0)
						base = 10;
					if(width > 0 && (flags&F_ZERO))
					{	/* do 0 padding first */
						if(fmt == 'x' || fmt == 'X')
							n = 0;
						else if(dot != 2)
							n = width;
						else	n = base < 10 ? 2 : 3;
						n += (flags&(F_MINUS|F_SIGN)) ? 1 : 0;
						n = width - (n + (endsp-sp));
						while(n-- > 0)
							*--sp = '0';
					}
					if(fmt == 'x' || fmt == 'X')
					{	*--sp = (char)fmt;
						*--sp = '0';
					}
					else if(dot == 2)
					{	/* base#value notation */
						*--sp = '#';
						if(base < 10)
							*--sp = (char)('0'+base);
						else
						{	*--sp = _Sfdec[(base <<= 1)+1];
							*--sp = _Sfdec[base];
						}
					}
				}
			}

			break;

		case 'g': /* %g and %G ultimately become %e or %f */
		case 'G':
		case 'e':
		case 'E':
		case 'f':
#if _typ_long_double
			if(flags&F_LDOUBLE)
			{	GETARG(f,t, dval,Sfdouble_t,Sfdouble_t, args,ftenv,dval,
					fmt, flags,n_lhL, base,precis, t_user,n_user );
			}
			else
#endif
			{	GETARG(f,t, dval,Sfdouble_t,double, args,ftenv,argrv.d,
					fmt, flags,n_lhL, base,precis, t_user,n_user );
			}

			if(fmt == 'e' || fmt == 'E')
			{	n = (precis = precis < 0 ? FPRECIS : precis)+1;
				ep = _sfcvt(dval,min(n,SF_FDIGITS),
					    &decpt,&sign,F_EFORMAT|(flags&F_LDOUBLE));
				goto e_format;
			}
			else if(fmt == 'f' || fmt == 'F')
			{	precis = precis < 0 ? FPRECIS : precis;
				ep = _sfcvt(dval,min(precis,SF_FDIGITS),
					    &decpt,&sign,(flags&F_LDOUBLE));
				goto f_format;
			}

			/* 'g' or 'G' format */
			precis = precis < 0 ? FPRECIS : precis == 0 ? 1 : precis;
			ep = _sfcvt(dval,min(precis,SF_FDIGITS),
				    &decpt,&sign,F_EFORMAT|(flags&F_LDOUBLE));
			if(dval == 0.)
				decpt = 1;
			else if(*ep == 'I')
				goto infinite;

			if(!(flags&F_ALTER))
			{	/* zap trailing 0s */
				if((n = sfslen()) > precis)
					n = precis;
				while((n -= 1) >= 1 && ep[n] == '0')
					;
				n += 1;
			}
			else	n = precis;

			flags = (flags & ~F_ZERO) | F_GFORMAT;
			if(decpt < -3 || decpt > precis)
			{	precis = n-1;
				goto e_format;
			}
			else
			{	precis = n - decpt;
				goto f_format;
			}

		e_format: /* build the x.yyyy string */
			if(isalpha(*ep))
				goto infinite;
			sp = endsp = buf+1;	/* reserve space for sign */
			*endsp++ = *ep ? *ep++ : '0';

			if(precis > 0 || (flags&F_ALTER))
				*endsp++ = GETDECIMAL(dc,locv);
			ssp = endsp;
			endep = ep+precis;
			while((*endsp++ = *ep++) && ep <= endep)
				;
			precis -= (endsp -= 1) - ssp;

			/* build the exponent */
			ep = endep = buf+(sizeof(buf)-1);
			if(dval != 0.)
			{	if((n = decpt - 1) < 0)
					n = -n;
				while(n > 9)
				{	v = n; n /= 10;	
					*--ep = (char)('0' + (v - n*10));
				}
			}
			else	n = 0;
			*--ep = (char)('0' + n);
			if(endep-ep <= 1)	/* at least 2 digits */
				*--ep = '0';

			/* the e/Exponent separator and sign */
			*--ep = (decpt > 0 || dval == 0.) ? '+' : '-';
			*--ep = isupper(fmt) ? 'E' : 'e';

			flags = (flags&~F_ZERO)|F_FLOAT;
			goto end_efg;

		f_format: /* data before the decimal point */
			if(isalpha(*ep))
			{
			infinite:
				endsp = (sp = ep)+sfslen();
				ep = endep;
				precis = 0;
				goto end_efg;
			}

			endsp = sp = buf+1;	/* save a space for sign */
			endep = ep+decpt;
			while(ep < endep && (*endsp++ = *ep++))
				;
			if(endsp == sp)
				*endsp++ = '0';

			if(precis > 0 || (flags&F_ALTER))
				*endsp++ = GETDECIMAL(dc,locv);

			if((n = -decpt) > 0)
			{	/* output zeros for negative exponent */
				ssp = endsp + min(n,precis);
				precis -= n;
				while(endsp < ssp)
					*endsp++ = '0';
			}

			ssp = endsp;
			endep = ep+precis;
			while((*endsp++ = *ep++) && ep <= endep)
				;
			precis -= (endsp -= 1) - ssp;
			ep = endep;
			flags |= F_FLOAT;
		end_efg:
			if(sign)
			{	/* if a %gG, output the sign now */
				if(flags&F_GFORMAT)
				{	*--sp = '-';
					flags &= ~(F_SIGN|F_BLANK);
				}
				else	flags |= F_MINUS;
			}
			break;
		}

		if(flags == 0)
			goto do_output;

		if(flags&(F_MINUS|F_SIGN|F_BLANK))
			fmt = (flags&F_MINUS) ? '-' : (flags&F_SIGN) ? '+' : ' ';

		n = (endsp-sp) + (endep-ep) + (precis <= 0 ? 0 : precis) +
		    ((flags&F_PREFIX) ? 1 : 0);
		if((v = width-n) <= 0)
			v = 0;
		else if(!(flags&F_ZERO)) /* right padding */
		{	if(flags&F_LEFT)
				v = -v;
			else if(flags&F_PREFIX) /* blank padding, output prefix now */
			{	*--sp = fmt;
				flags &= ~F_PREFIX;
			}
		}

		if(flags&F_PREFIX) /* put out the prefix */
		{	SFputc(f,fmt);
			if(fmt != ' ')
				flags |= F_ZERO;
		}

		if((n = v) > 0) /* left padding */
		{	v = (flags&F_ZERO) ? '0' : ' ';
			SFnputc(f,v,n);
		}

		if((n = precis) > 0 && ((flags&F_REPEAT) || !(flags&F_FLOAT)))
		{	/* padding for integer precision */
			v = (flags&F_REPEAT) ? fmt : '0';
			SFnputc(f,v,n);
			precis = 0;
		}

	do_output:
		if((n = endsp-sp) > 0)
			SFwrite(f,sp,n);

		if(flags&(F_FLOAT|F_LEFT))
		{	/* F_FLOAT: right padding for float precision */
			if((n = precis) > 0)
				SFnputc(f,'0',n);

			/* F_FLOAT: the exponent of %eE */
			if((n = endep-(sp=ep)) > 0)
				SFwrite(f,sp,n);

			/* F_LEFT: right padding */
			if((n = -v) > 0)
				SFnputc(f,' ',n);
		}
	}

pop_fmt:
	if((fm = fmstk) ) /* pop the format stack and continue */
	{	fmstk = fm->next;
		if((form = fm->form) )
			va_copy(args, fm->args);
		ftenv = fm->ftenv;
		FMTFREE(fm);
		if(form)
			goto loop_fmt;
	}

done:
	while((fm = fmstk) )
	{	fmstk = fm->next;
		FMTFREE(fm);
	}

	SFEND(f);

	n = f->next - f->data;
	if((d = (char*)f->data) == data)
		f->endw = f->endr = f->endb = f->data = NIL(uchar*);
	f->next = f->data;

	if((((flags = f->flags)&SF_SHARE) && !(flags&SF_PUBLIC) ) ||
	   (n > 0 && (d == data || (flags&SF_LINE) ) ) )
		(void)SFWRITE(f,(Void_t*)d,n);
	else	f->next += n;

	SFOPEN(f,0);
	return n_output;
}
