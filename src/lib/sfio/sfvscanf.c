#include	"sfhdr.h"

/*	The main engine for reading formatted data
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#define F_MINUS			000010			/* minus sign detected	*/
#define F_ALTER			000020			/* # sign detected	*/
#define MAXWIDTH		(int)(((uint)~0)>>1)	/* max amount to scan	*/

#define SETARG(f,t, val,val_t, args,fe, fmt, flg,n_flg, base, typ,n_typ) \
	{ if(fe && fe->argf) \
	  { FMTPARAM(fe, fmt, flg,n_flg, base,sizeof(val_t), typ,n_typ); \
	    if((t = (*fe->argf)(sf, (Void_t*)(&(val)), ftenv)) <= 0) \
	    { if(t == 0)	*va_arg(args,val_t*) = (val_t)val; \
	      else		goto pop_fmt; \
	    } \
	  } else		*va_arg(args,val_t*) = (val_t)val; \
	}

#define A_SIZE		(1<<8)		/* alphabet size		*/

static uchar		V36[A_SIZE];	/* digit value for base <= 36	*/
static uchar		V64[A_SIZE];	/* digit value for base <= 64	*/
static int		Tabinit = 0;	/* 1 if table has been init	*/

/* thread-safe macro/function to initialize conversion tables */
#define TABINIT()	(Tabinit ? 1 : (Tabinit = tabinit()) )
static int tabinit()
{	reg int	d, l;

	for(d = 0; d < A_SIZE; ++d)
		V36[d] = V64[d] = 64;

	/* [0-9] */
	for(d = 0; d < 10; ++d)
		V36[(uchar)_Sfdigits[d]] = V64[(uchar)_Sfdigits[d]] = d;

	/* [a-z] */
	for(; d < 36; ++d)
		V36[(uchar)_Sfdigits[d]] = V64[(uchar)_Sfdigits[d]] = d;

	/* [A-Z] */
	for(l = 10; d < 62; ++l, ++d)
	{	V36[(uchar)_Sfdigits[d]] = l;
		V64[(uchar)_Sfdigits[d]] = d;
	}

	/* whatever left */
	for(; d < 64; ++d)
		V36[(uchar)_Sfdigits[d]] = V64[(uchar)_Sfdigits[d]] = d;

	return 1;
}

#if __STD_C
static char* setclass(reg char* form, reg char* accept)
#else
static char* setclass(form,accept)
reg char*	form;	/* format string			*/
reg char*	accept;	/* accepted characters are set to 1	*/
#endif
{
	reg int		fmt, c, yes;

	if((fmt = *form++) == '^')
	{	/* we want the complement of this set */
		yes = 0;
		fmt = *form++;
	}
	else	yes = 1;

	for(c = 0; c < A_SIZE; ++c)
		accept[c] = !yes;

	if(fmt == ']' || fmt == '-')
	{	/* special first char */
		accept[fmt] = yes;
		fmt = *form++;
	}

	for(; fmt != ']'; fmt = *form++)
	{	/* done */
		if(!fmt)
			return (form-1);

		/* interval */
		if(fmt != '-' || form[0] == ']' || form[-2] > form[0])
			accept[fmt] = yes;
		else for(c = form[-2]+1; c < form[0]; ++c)
			accept[c] = yes;
	}

	return form;
}

#if __STD_C
static void _sfbuf(Sfio_t* f, int* rs)
#else
static void _sfbuf(f, rs)
Sfio_t*	f;
int*	rs;
#endif
{
	if(f->next >= f->endb)
	{	if(*rs > 0)	/* try peeking for a share stream if possible */
		{	f->mode |= SF_RV;
			if(SFFILBUF(f,-1) > 0)
			{	f->mode |= SF_PEEK;
				return;
			}
			*rs = -1;	/* can't peek, back to normal reads */
		}
		(void)SFFILBUF(f,-1);
	}
}

#if __STD_C
int sfvscanf(Sfio_t* f, reg const char* form, va_list args)
#else
int sfvscanf(f,form,args)
Sfio_t*		f;		/* file to be scanned */
reg char*	form;		/* scanning format */
va_list		args;
#endif
{
	reg uchar	*d, *endd, *data;
	reg int		inp, shift, base, width, len, t;
	int		fmt, skip, flag, n_flag, n_assign;
	Sfio_t*		sf;
	reg char*	sp;
	int		n_input;
	char		accept[SF_MAXDIGITS];
	Argrv_t		argrv;
	Sffmt_t		*ft, *ftenv;
	Fmt_t		*fm, *fmstk;
	char*		t_user;
	int		n_user;
	int		rs = (f->extent < 0 && (f->flags&SF_SHARE)) ? 1 : 0;

#define SFBUF(f)	(_sfbuf(f,&rs), (data = d = f->next), (endd = f->endb) )
#define SFLEN(f)	(d-data)
#define SFEND(f)	((n_input += d-data), \
			 (rs > 0 ? SFREAD(f,(Void_t*)data,d-data) : ((f->next = d), 0)) )
#define SFGETC(f,c)	((c) = (d < endd || (SFEND(f), SFBUF(f), d < endd)) ? \
				(int)(*d++) : -1 )
#define SFUNGETC(f,c)	(--d)

	if(f->mode != SF_READ && _sfmode(f,SF_READ,0) < 0)
		return -1;
	SFLOCK(f,0);

	TABINIT();	/* initialize conversion table */

	SFBUF(f);
	n_assign = n_input = 0;
	inp = -1;
	sf = NIL(Sfio_t*);
	fmstk = NIL(Fmt_t*);
	ftenv = NIL(Sffmt_t*);

loop_fmt :
	while((fmt = *form++) )
	{	if(fmt != '%')
		{	if(isspace(fmt))
			{	if(fmt != '\n' || !(f->flags&SF_LINE))
					fmt = -1;
				for(;;)
				{	if(SFGETC(f,inp) < 0)
						goto done;
					else if(!isspace(inp))
					{	SFUNGETC(f,inp);
						break;
					}
					else if(inp == fmt)	/* match only one \n */
						break;
				}
			}
			else
			{ literal:
				if(SFGETC(f,inp) != fmt)
				{	if(inp >= 0)
						SFUNGETC(f,inp);
					goto pop_fmt;
				}
			}
			continue;
		}

		/* matching some pattern */
		base = 10;
		skip = width = n_flag = flag = 0;
		t_user = NIL(char*); n_user = 0;
		switch((fmt = *form++) )
		{
		case '%' : /* match % literally */
			goto literal;

		case 'n' : /* return number of bytes read */
			argrv.i = n_input + SFLEN(f);
			SETARG(f,t, argrv.i,int, args,ftenv,
				'n', 0,0, base, t_user,n_user);
			continue;

		case '!' :
			if(!(ft = va_arg(args,Sffmt_t*)) )
				goto pop_fmt;

			if(ft->form || !(fm = fmstk) )
			{	if(!FMTALLOC(fm))
					goto done;
				if(ft->form)
				{	fm->form = (char*)form;
					va_copy(fm->args, args);
					form = ft->form;
					va_copy(args, ft->args);
				}
				else	fm->form = NIL(char*);

				fm->ftenv = ftenv;
				fm->next = fmstk;
				fmstk = fm;
			}

			ftenv = ft;
			break;

		case '*' :	/* skip one argument */
			skip = 1;
			fmt = *form++;

		default :
			while(isdigit(fmt))	/* max amount to scan 	*/
			{	width = width*10 + (fmt - '0');
				fmt = *form++;
			}

			if(fmt == '.')		/* int conversion base	*/
			{	fmt = *form++;
				base = 0;
				while(isdigit(fmt))
				{	base = base*10 + (fmt - '0');
					fmt = *form++;
				}
				if(base < 2 || base > SF_RADIX)
					base = 10;
			}

			if(fmt == '(')		/* type in balanced parens */
			{	t_user = (char*)form;
				for(t = 1; t != 0;)
				{	switch(*form++)
					{
					case 0 :	/* not balancable, retract */
						form = t_user;
						t_user = NIL(char*);
						n_user = 0;
						t = 0;
						break;
					case '(' :	/* increasing nested level */
						t += 1;
						break;
					case ')' :	/* decreasing nested level */
						if((t -= 1) == 0)
							n_user = (form-1) - t_user;
						break;
					}
				}
			}

			/* flag indicating size of object to be assigned */
			flag = n_flag = 0;
			while(fmt == 'h' || fmt == 'l' || fmt == 'L' || fmt == '#')
			{	if(fmt == 'h')
				{	if(flag&F_SHORT)
						n_flag += 1;
					else
					{	flag = (flag&~F_TYPES)|F_SHORT;
						n_flag = 1;
					}
				}
				else if(fmt == 'l')
				{	if(flag&F_LONG)
						n_flag += 1;
					else
					{	flag = (flag&~F_TYPES)|F_LONG;
						n_flag = 1;
					}
				}
				else if(fmt == 'L')
				{	if(flag&F_LDOUBLE)
						n_flag += 1;
					else
					{	flag = (flag&~F_TYPES)|F_LDOUBLE;
						n_flag = 1;
					}
				}
				else /* if(fmt == '#') */
				{	flag |= F_ALTER;
				}

				fmt = *form++;
			}

			/* canonicalize format */
			if(fmt == 'e' || fmt == 'g' || fmt == 'E' || fmt == 'G')
				fmt = 'f';
			else if(fmt == 'X')
				fmt = 'x';
		}

		if(width == 0)
			width = fmt == 'c' ? 1 : MAXWIDTH;

		/* define the first input character */
		if(fmt == 'c' || fmt == '[')
			SFGETC(f,inp);
		else
		{	/* skip starting blanks */
			do	{ SFGETC(f,inp); }
			while(isspace(inp))
				;
		}
		if(inp < 0)
			goto done;

		if(fmt == 'f')	/* float, double, long double */
		{	reg char*	val;
			reg int		dot, exponent;

			val = accept;
			if(width >= SF_MAXDIGITS)
				width = SF_MAXDIGITS-1;
			dot = exponent = 0;
			do
			{	if(isdigit(inp))
					*val++ = inp;
				else if(inp == '.')
				{	/* too many dots */
					if(dot++ > 0)
						break;
					*val++ = '.';
				}
				else if(inp == 'e' || inp == 'E')
				{	/* too many e,E */
					if(exponent++ > 0)
						break;
					*val++ = inp;
					if(--width <= 0 || SFGETC(f,inp) < 0 ||
					   (inp != '-' && inp != '+' && !isdigit(inp)) )
						break;
					*val++ = inp;
				}
				else if(inp == '-' || inp == '+')
				{	/* too many signs */
					if(val > accept)
						break;
					*val++ = inp;
				}
				else	break;

			} while(--width > 0 && SFGETC(f,inp) >= 0);

			if(!skip)
			{	n_assign += 1;
				*val = '\0';
				if(!(flag&(F_LDOUBLE|F_LONG)) )
				{	argrv.f = (float)strtod(accept,NIL(char**));
				set_f:	SETARG(f,t, argrv.f,float, args,ftenv,
						'f', flag,n_flag, base, t_user,n_user);
				}
#if _typ_long_double
				else if(flag&F_LDOUBLE)
				{	argrv.ld = _sfstrtod(accept,NIL(char**));
				set_ld:	SETARG(f,t, argrv.ld,Sfdouble_t, args,ftenv,
						'f', flag,n_flag, base, t_user,n_user);
				}
#endif
				else
				{	argrv.d = (double)strtod(accept,NIL(char**));
				set_d:	SETARG(f,t, argrv.d,double, args,ftenv,
						'f', flag,n_flag, base, t_user,n_user);
				}
			}
		}
		else if(fmt == 'p' || fmt == 'u' || fmt == 'o' || fmt == 'x' )
		{	if(inp == '-')
			{	SFUNGETC(f,inp);
				goto pop_fmt;
			}
			else	goto dec_convert;
		}
		else if(fmt == 'i' || fmt == 'd' )
		{ dec_convert:
			if(inp == '-' || inp == '+')
			{	if(inp == '-')
					flag |= F_MINUS;
				while(--width > 0 && SFGETC(f,inp) >= 0)
					if(!isspace(inp))
						break;
			}
			if(inp < 0)
				goto done;

			if(fmt == 'i') /* data type is self-described */
			{	if(inp != '0')
					base = 10;
				else
				{	if(--width > 0)
						SFGETC(f,inp);
					if(inp == 'x' || inp == 'X')
					{	base = 16;
						if(--width > 0)
							SFGETC(f,inp);
					}
					else	base = 8;
				}
			}
			else if(fmt == 'o')
				base = 8;
			else if(fmt == 'x' || fmt == 'p')
				base = 16;

			/* now convert */
			argrv.lu = 0;
			if(base == 16)
			{	sp = (char*)V36;
				shift = 4;
				if(sp[inp] >= 16)
				{	SFUNGETC(f,inp);
					goto pop_fmt;
				}
				if(inp == '0' && --width > 0)
				{	/* skip leading 0x or 0X */
					SFGETC(f,inp);
					if((inp == 'x' || inp == 'X') && --width > 0)
						SFGETC(f,inp);
				}
				if(inp >= 0 && sp[inp] < 16)
					goto base_shift;
			}
			else if(base == 10)
			{	/* fast base 10 conversion */
				if(inp < '0' || inp > '9')
				{	SFUNGETC(f,inp);
					goto pop_fmt;
				}

				do
				{ argrv.lu = (argrv.lu<<3) + (argrv.lu<<1) + (inp-'0');
				} while(--width > 0 &&
				        SFGETC(f,inp) >= '0' && inp <= '9');

				if(fmt == 'i' && inp == '#' && !(flag&F_ALTER) )
				{	base = argrv.lu;
					if(base < 2 || base > SF_RADIX)
						goto pop_fmt;
					argrv.lu = 0;
					sp = base <= 36 ? (char*)V36 : (char*)V64;
					if(--width > 0 &&
					   SFGETC(f,inp) >= 0 && sp[inp] < base)
						goto base_conv;
				}
			}
			else
			{	/* other bases */
				sp = base <= 36 ? (char*)V36 : (char*)V64;
				if(base < 2 || base > SF_RADIX || sp[inp] >= base)
				{	SFUNGETC(f,inp);
					goto pop_fmt;
				}

			base_conv: /* check for power of 2 conversions */
				if((base & ~(base-1)) == base)
				{	if(base < 8)
						shift = base <  4 ? 1 : 2;
					else if(base < 32)
						shift = base < 16 ? 3 : 4;
					else	shift = base < 64 ? 5 : 6;

			base_shift:	do
					{ argrv.lu = (argrv.lu << shift) + sp[inp];
					} while(--width > 0 &&
					        SFGETC(f,inp) >= 0 && sp[inp] < base);
				}
				else
				{	do
					{ argrv.lu = (argrv.lu * base) + sp[inp];
					} while(--width > 0 &&
						SFGETC(f,inp) >= 0 && sp[inp] < base);
				}
			}

			if(!skip)
			{	n_assign += 1;
				if(fmt == 'p')
				{
#if _more_void_int
					argrv.vp = (Void_t*)argrv.lu;
#else
					argrv.vp = (Void_t*)((ulong)argrv.lu);
#endif
				set_p:	SETARG(f,t, argrv.vp,Void_t*, args,ftenv,
						fmt, n_flag,flag, base, t_user,n_user);
				}
				else if(!(flag&(F_SHORT|F_LONG)) )
				{	argrv.i = (int)argrv.lu;
					if(flag&F_MINUS)
						argrv.i = -argrv.i;
				set_i:	SETARG(f,t, argrv.i,int, args,ftenv,
						fmt, n_flag,flag, base, t_user,n_user);
				}
				else if(flag&F_SHORT)
				{	argrv.h = (short)((ushort)argrv.lu);
					if(flag&F_MINUS)
						argrv.h = -argrv.h;
				set_h:	SETARG(f,t, argrv.h,short, args,ftenv,
						fmt, n_flag,flag, base, t_user,n_user);
				}
#if _typ_long_long
				else if(n_flag >= 2)
				{	argrv.ll = (Sflong_t)argrv.lu;
					if(flag&F_MINUS)
						argrv.ll = -argrv.ll;
				set_ll:	SETARG(f,t, argrv.ll,Sflong_t, args,ftenv,
						fmt, n_flag,flag, base, t_user,n_user);
				}
#endif
				else
				{	argrv.l = (long)argrv.lu;
					if(flag&F_MINUS)
						argrv.l = -argrv.l;
				set_l:	SETARG(f,t, argrv.l,long, args,ftenv,
						fmt, n_flag,flag, base, t_user,n_user);
				}
			}
		}
		else if(fmt == 's' || fmt == 'c' || fmt == '[')
		{	len = 0;
			argrv.s = NIL(char*);
			if(!skip)
			{	if(ftenv && ftenv->argf) /* collect data in a string */
				{	if(sf)
						sfseek(sf,0,0);
					else
					{	sf = sfnew(NIL(Sfio_t*),
							   NIL(char*),(size_t)SF_UNBOUND,
							   -1, SF_STRING|SF_WRITE);
						if(!sf)
							goto done;
					}
				}
				else		/* write into provided buffer */
				{	if(!(argrv.s = va_arg(args,char*)) )
						goto done;

					if(!(flag&F_ALTER) )
						len = MAXWIDTH;
					else
					{	if((len = va_arg(args,int)) < 0)
							argrv.s = NIL(char*);
						else
						{	if(fmt != 'c')
								len -= 1;
							if(len <= 0)
							{	len = 0;
								skip = -1;
							}
						}
					}
				}
			}

			t = width;
			if(fmt == 's')
			{	do
				{	if(isspace(inp))
						break;
					if(len)
					{	*argrv.s++ = inp;
						if((len -= 1) == 0)
							skip = -1;
					}
					else if(!skip && sf)
						sfputc(sf,inp);
				} while(--width > 0 && SFGETC(f,inp) >= 0);
			}
			else if(fmt == 'c')
			{	do
				{	if(len)
					{	*argrv.s++ = inp;
						if((len -= 1) == 0)
							skip = -1;
					}
					else if(!skip && sf)
						sfputc(sf,inp);
				} while(--width > 0 && SFGETC(f,inp) >= 0);
			}
			else /* if(fmt == '[') */
			{	form = setclass((char*)form,accept);
				do
				{	if(!accept[inp])
						break;
					if(len)
					{	*argrv.s++ = inp;
						if((len -= 1) == 0)
							skip = -1;
					}
					else if(!skip && sf)
						sfputc(sf,inp);
				} while(--width > 0 && SFGETC(f,inp) >= 0);
			}

			if(skip <= 0 && width < t)
			{	n_assign += 1;
				if(ftenv && ftenv->argf)
				{	sfputc(sf,'\0');
					argrv.s = (char*)sf->data;
					len = (sf->next-sf->data)-1;
					goto s_assign;
				}
				else if(argrv.s && fmt != 'c')
					*argrv.s = '\0';
			}
		}
		else /* undefined pattern */
		{	/* return the read byte to the stream */
			SFUNGETC(f,inp);
			if(ftenv && ftenv->extf)
			{	/* call extension function */
				SFEND(f);
				FMTPARAM(ftenv,fmt,flag,n_flag,base,0,t_user,n_user);
				t = (*ftenv->extf)(f, (Void_t*)(&argrv), width, ftenv);
				SFBUF(f);
				if(t < 0)
					goto pop_fmt;
				else if(t == 0)
					continue;

				if(skip)
					continue;

				fmt = ftenv->fmt;
				n_assign += 1;
				switch(fmt)
				{
				case 'p' :
					goto set_p;

				case 'i' : case 'd' : case 'u' :
				case 'o' : case 'x' : case 'X' :
					if(!(flag&(F_SHORT|F_LONG)) )
						goto set_i;
					else if(flag&F_SHORT)
						goto set_h;
#if _typ_long_long
					else if(n_flag >= 2)
						goto set_ll;
#endif
					else	goto set_l;

				case 'f' : case 'e' : case 'g' :
				case 'E' : case 'G' :
					if(!(flag&(F_LONG|F_LDOUBLE)) )
						goto set_f;
#if _typ_long_double
					else if(flag&F_LDOUBLE)
						goto set_ld;
#endif
					else	goto set_d;

				case 'c' : case 's' : case '[' :
					len = strlen(argrv.s);
				s_assign :
					if(ftenv && ftenv->argf)
					{	FMTPARAM(ftenv, fmt, flag,n_flag,
							base,len, t_user,n_user);
						t = (*ftenv->argf)
							(f,(Void_t*)argrv.s,ftenv);
						if(t < 0)
							goto pop_fmt;
						else if(t == 0)
							goto s_buffer;
					}
					else
					{ s_buffer:
						if(!(sp = va_arg(args,char*)) )
							goto done;

						if(!(flag&F_SHORT) )
							t = MAXWIDTH;
						else
						{	t = va_arg(args,int);
							if(fmt != 'c' && t > 0)
								t -= 1;
						}
						if(len > t)
							len = t;
						memcpy((Void_t*)sp,
							(Void_t*)argrv.s, len);
						if(fmt != 'c')
							sp[len] = '\0';
					}
					break;
				}
			}
			continue;
		}

		if(width > 0 && inp >= 0)
			SFUNGETC(f,inp);
	}

pop_fmt:
	if((fm = fmstk) )
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

	if(sf)
		sfclose(sf);

	SFEND(f);
	SFOPEN(f,0);
	return (n_assign == 0 && inp < 0) ? -1 : n_assign;
}
