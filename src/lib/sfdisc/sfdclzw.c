#include	"sfdchdr.h"


/*
 * compress.c - File compression ala IEEE Computer, June 1984.
 *
 * Authors:	Spencer W. Thomas	(decvax!utah-cs!thomas)
 *		Jim McKie		(decvax!mcvax!jim)
 *		Steve Davies		(decvax!vax135!petsd!peora!srd)
 *		Ken Turkowski		(decvax!decwrl!turtlevax!ken)
 *		James A. Woods		(decvax!ihnp4!ames!jaw)
 *		Joe Orost		(decvax!vax135!petsd!joe)
 *
 * July, 1992, Jim Arnold
 *	Modified uncompress code to work as a discipline under sfio.
 *	Didn't need compression code and deleted it.
 *
 * Mar, 1993, (Kiem-)Phong Vo
 *	Small interface modifications to conform with other disciplines.
 */

/*****************************************************************
 * Algorithm from "A Technique for High Performance Data Compression",
 * Terry A. Welch, IEEE Computer Vol 17, No 6 (June 1984), pp 8-19.
 *
 * Algorithm:
 * 	Modified Lempel-Ziv method (LZW).  Basically finds common
 * substrings and replaces them with a variable size code.  This is
 * deterministic, and can be done on the fly.  Thus, the decompression
 * procedure needs no input table, but tracks the way the table was built.
 */

/*
/*static	char sccsid[] = "@(#)compress.c 1.1 90/10/29 SMI"; /* from UCB 5.9 5/11/86 */
/*static char rcs_ident[] = "$Header: compress.c,v 4.0 85/07/30 12:50:00 joe Release $";
/**/


#ifndef BITS
#	define BITS	16
#endif
#define INIT_BITS	9		/* initial number of bits/code */


/*	Defines for third byte of header.
 *	Masks 0x40 and 0x20 are free.  I think 0x20 should mean that there is
 *	a fourth header byte (for expansion).  If the BLOCK_MASK is set,
 *	the CODE_CLEAR is disabled.  Instead of flushing all the codes,
 *	they are simply overwritten.
 */

#define BIT_MASK	0x1f
#define BLOCK_MASK	0x80


/*	The next two codes should not be changed lightly, as they must not
 *	lie within the contiguous general code space.
 */ 

#define CODE_FIRST	257	/* first free entry */
#define	CODE_CLEAR	256	/* table clear output code */
#define MAXCODE(n_bits)	((1 << (n_bits)) - 1)


/*	A code_int must hold 2**BITS non-negative values, and also -1
 */

#if BITS > 15
typedef long int	code_int;
#else
typedef int		code_int;
#endif
typedef	unsigned char	char_type;


typedef struct
{
	Sfdisc_t	disc;
	int		init;
	int		n_bits;		/* number of bits/code */
	int		maxbits;	/* user settable max # bits/code */
	int		block_compress;
	code_int	maxcode;	/* maximum code, given n_bits */
	code_int	maxmaxcode;	/* should NEVER generate this code */
	code_int	free_ent;	/* first unused entry */
	int		clear_flg;
	int		finchar;
	char_type*	stackp;
	code_int	code;
	code_int	oldcode;
	code_int	incode;
	int		gc_offset;	/* getcode() */
	int		gc_size;	/* getcode() */
	char_type*	gc_buf;		/* getcode() */
	char_type*	io_ptr;
	char_type*	io_end;
	char		io_buf[BITS + 8192];
	char_type	de_stack[8000];
	char_type	tab_suffix [1 << BITS];
	unsigned short	tab_prefix [1 << BITS];
} LZW_Disc;


static char_type rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

#ifdef __STD_C
static int peek(Sfio_t* f, char_type** bufp, int count, reg LZW_Disc* disc)
#else
static int peek(f, bufp, count, disc)
Sfio_t*		f;
char_type**	bufp;
int		count;
reg LZW_Disc*	disc;
#endif
{
	reg int	io_sz, j;

	if (count <= 0)
		return count;
	if (count > BITS)
		return -1;
	if ((io_sz = disc->io_end - disc->io_ptr) < count)
	{
		memcpy(disc->io_buf + BITS - io_sz, disc->io_ptr, io_sz);
		disc->io_ptr = (char_type *)disc->io_buf + BITS - io_sz;
		j = sfrd(f, disc->io_buf + BITS, sizeof disc->io_buf - BITS, (Sfdisc_t *)disc);
		if (j < 0)
			j = 0;
		io_sz += j;
		disc->io_end = disc->io_ptr + io_sz;
	}
	*bufp = disc->io_ptr;
	if (io_sz < count)
		count = io_sz;
	disc->io_ptr += count;
	return count;
}


/*****************************************************************
 * TAG( getcode )
 *
 * Read one code from the standard input.  If EOF, return -1.
 * Inputs:
 * 	stdin
 * Outputs:
 * 	code or -1 is returned.
 */

#ifdef __STD_C
static code_int	getcode(Sfio_t* f, LZW_Disc* disc)
#else
static code_int getcode(f, disc)
Sfio_t*		f;
LZW_Disc*	disc;
#endif
{
	reg code_int	code;
	reg int		r_off, bits;
	reg char_type	*bp;

	if ( disc->clear_flg > 0
	|| disc->gc_offset >= disc->gc_size
	|| disc->free_ent > disc->maxcode ) {
		/*
		 * If the next entry will be too big for the current code
		 * size, then we must increase the size.  This implies reading
		 * a new buffer full, too.
		 */
		if ( disc->free_ent > disc->maxcode ) {
			if (++disc->n_bits > disc->maxbits)
				return -1;
			if ( disc->n_bits == disc->maxbits )
				disc->maxcode = disc->maxmaxcode;
			else
				disc->maxcode = MAXCODE(disc->n_bits);
		}
		if ( disc->clear_flg > 0) {
    			disc->maxcode = MAXCODE (disc->n_bits = INIT_BITS);
			disc->clear_flg = 0;
		}
		disc->gc_size = peek(f, &disc->gc_buf, disc->n_bits, disc);
		if ( disc->gc_size <= 0 )
			return -1;			/* end of file */
		disc->gc_offset = 0;
		/* Round size down to integral number of codes */
		disc->gc_size = (disc->gc_size << 3) - (disc->n_bits - 1);
	}
	bp = disc->gc_buf;
	r_off = disc->gc_offset;
	bits = disc->n_bits;
	/*
	 * Get to the first byte.
	 */
	bp += (r_off >> 3);
	r_off &= 7;
	/* Get first part (low order bits) */
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;		/* now, offset into code word */
	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if ( bits >= 8 ) {
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}
	/* high order bits. */
	code |= (*bp & rmask[bits]) << r_off;
	disc->gc_offset += disc->n_bits;
	return code;
}


#ifdef __STD_C
static int lzwExcept(Sfio_t* f, int type, Sfdisc_t* disc)
#else
static int lzwExcept(f, type, disc)
Sfio_t*		f;
int		type;
Sfdisc_t*	disc;
#endif
{
	reg Sfdisc_t*	pop;

	if(type == SF_CLOSE)
	{
		if ((pop = sfdisc(f, SF_POPDISC)) != disc)
			sfdisc(f, pop);	/* shouldn't happen */
		else	sfdcdellzw(disc);
	}
	return 0;
}


/*
 * Uncompress.  This routine adapts to the codes in the
 * file building the "string" table on-the-fly; requiring no table to
 * be stored in the compressed file.  The tables used herein are shared
 * with those of the compress() routine.  See the definitions above.
 */

#ifdef __STD_C
lzwRead(Sfio_t* f, char* iobuf, int iocnt, Sfdisc_t* sfdisc)
#else
lzwRead(f, iobuf, iocnt, sfdisc)
Sfio_t*		f;
char*		iobuf;
int		iocnt;
Sfdisc_t*	sfdisc;
#endif
{
	LZW_Disc	*disc = (LZW_Disc *)sfdisc;
	reg char_type	*stackp;
	reg code_int	code;
	char		*ioend = iobuf + iocnt;
	register char	*ioptr = iobuf;
#define END_REGS	{disc->code=code;disc->stackp=stackp;}
#define BEGIN_REGS	{code=disc->code;stackp=disc->stackp;}

	BEGIN_REGS
	if (disc->init <= 0)
	{
		char_type	*p;

		if (disc->init < 0)
			return disc->init;
		if (iocnt <= 0)
			return iocnt;
		/* Check the magic number */
		if (peek(f, &p, 3, disc) != 3
		|| *p++ != (char_type)0x1f
		|| *p++ != (char_type)0x9d)
			return disc->init = -1;
		disc->maxbits = *p;		/* set -b from file */
		disc->block_compress = disc->maxbits & BLOCK_MASK;
		disc->maxbits &= BIT_MASK;
		disc->maxmaxcode = 1 << disc->maxbits;
		if(disc->maxbits > BITS)
			return disc->init = -1;
		disc->init = 1;

		/*
		 * As above, initialize the first 256 entries in the table.
		 */
		disc->maxcode = MAXCODE(disc->n_bits = INIT_BITS);
		for ( code = 255; code >= 0; code-- ) {
			disc->tab_prefix[code] = 0;
			disc->tab_suffix[code] = (char_type)code;
		}
		disc->free_ent = ((disc->block_compress) ? CODE_FIRST : 256 );

		stackp = disc->de_stack;
		disc->finchar = disc->oldcode = getcode(f, disc);
		if(disc->oldcode == -1)		/* EOF already? */
		{
			END_REGS
			return 0;		/* Get out of here */
		}
		*ioptr++ = (char)disc->finchar;
		if ((code = getcode(f, disc)) < 0)
		{
			END_REGS
			return 1;
		}
	}

	do
	{
		if (stackp <= disc->de_stack)
		{
			if ( (code == CODE_CLEAR) && disc->block_compress ) {
				for ( code = 255; code >= 0; code-- )
					disc->tab_prefix[code] = 0;
				disc->clear_flg = 1;
				disc->free_ent = CODE_FIRST - 1;
				if ( (code = getcode (f, disc)) == -1 )
					break;
			}
			else if (code < 0)
				break;
			disc->incode = code;
			/*
			 * Special case for KwKwK string.
			 */
			if ( code >= disc->free_ent ) {
				*stackp++ = disc->finchar;
				code = disc->oldcode;
			}

			/*
			 * Generate output characters in reverse order
			 */
			while ( code >= 256 ) {
				*stackp++ = disc->tab_suffix[code];
				code = disc->tab_prefix[code];
			}
			*stackp++ = disc->finchar = disc->tab_suffix[code];
		}

		/*
		 * And put them out in forward order
		 */
		do
		{
			if (ioptr >= ioend)
			{
				END_REGS
				return iocnt;
			}
			*ioptr++ = *--stackp;
		}while ( stackp > disc->de_stack );

		/*
		 * Generate the new entry.
		 */
		if ( (code=disc->free_ent) < disc->maxmaxcode ) {
			disc->tab_prefix[code] = (unsigned short)disc->oldcode;
			disc->tab_suffix[code] = disc->finchar;
			disc->free_ent = code+1;
		} 
		/*
		 * Remember previous code.
		 */
		disc->oldcode = disc->incode;
	} while ((code = getcode(f, disc)) >= 0);
	END_REGS
	return ioptr - iobuf;
}

#ifdef __STD_C
static long lzwSeek(Sfio_t* f, long offset, int whence, Sfdisc_t* disc)
#else
static long lzwSeek(f, offset, whence, disc)
Sfio_t*		f;
long		offset;
int		whence;
Sfdisc_t*	disc;
#endif
{
	return -1L;
}


#ifdef __STD_C
static int lzwWrite(Sfio_t* f, const char* buf, int count, Sfdisc_t* disc)
#else
static int lzwWrite(f, buf, count, disc)
Sfio_t*		f;
char*		buf;
int		count;
Sfdisc_t*	disc;
#endif
{
	return -1;
}


Sfdisc_t* sfdcnewlzw()
{
	LZW_Disc*	disc;

	if (!(disc = (LZW_Disc *)malloc(sizeof(LZW_Disc))) )
		return NIL(Sfdisc_t*);
	disc->disc.readf = lzwRead;
	disc->disc.writef = lzwWrite;
	disc->disc.seekf = lzwSeek;
	disc->disc.exceptf = lzwExcept;
	disc->init = 0;
	disc->clear_flg = 0;
	disc->gc_offset = 0;
	disc->gc_size = 0;
	disc->io_ptr = (char_type *)disc->io_buf + BITS;
	disc->io_end = (char_type *)disc->io_buf + BITS;
	return (Sfdisc_t *)disc;
}


#ifdef __STD_C
sfdcdellzw(Sfdisc_t* disc)
#else
sfdcdellzw(disc)
Sfdisc_t*	disc;
#endif
{
	free((char*)disc);
	return 0;
}


#ifdef PROGRAM
/*	The following program is uncompress.
*/
main()
{
	Sfdisc_t*	disc;

	if(!(disc = sfdcnewlzw()) )
		return -1;

	sfdisc(sfstdin,disc);
	sfmove(sfstdin,sfstdout,-1,-1);

	return 0;
}
#endif /*PROGRAM*/
