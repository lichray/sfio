#include	"sfhdr.h"

/*	Poll a set of streams to see if any is available for I/O.
**	Ready streams are moved to front of array but retain the
**	same relative order.
**
**	Written by Kiem-Phong Vo.
*/

#if __STD_C
int sfpoll(Sfio_t** fa, reg int n, int tm)
#else
int sfpoll(fa, n, tm)
Sfio_t**	fa;	/* array of streams to poll		*/
reg int		n;	/* number of streams in array		*/
int		tm;	/* time in millisecs for select/poll	*/
#endif
{
	reg int		r, c, m, np;
	reg Sfio_t*	f;
	reg Sfdisc_t*	d;
	reg int		*status, *check;

	if(n <= 0 || !fa)
		return -1;

	if(!(status = (int*)malloc(2*n*sizeof(int))) )
		return -1;
	check = status+n; /* streams that need polling */

/* check the auxilliary file descriptor to see if polling needed */
#define CHKAUXFD(f)	((f->mode&SF_WRITE) && f->proc && f->proc->file >= 0 && \
			 f->proc->file != f->file && f->proc->rdata == 0 )

/* check the main file descriptor to see if polling needed */
#define POLLRD(f)	((f->mode&SF_READ)  && f->next == f->endb )
#define POLLWR(f)	((f->mode&SF_WRITE) && f->next >  f->data )
#define CHKMAINFD(f)	(POLLRD(f) || POLLWR(f))

	for(r = c = 0; r < n; ++r) /* compute streams that must be checked */
	{	f = fa[r];
		status[r] = 0;

		/* check accessibility */
		m = f->mode&SF_RDWR;
		if((int)f->mode != m && _sfmode(f,m,0) < 0)
			continue;

		if((f->flags&SF_READ)  && !POLLRD(f) && !CHKAUXFD(f) )
			status[r] |= SF_READ;

		if((f->flags&SF_WRITE) && (!POLLWR(f) || (f->mode&SF_READ)) )
			status[r] |= SF_WRITE;

		if((f->flags&SF_RDWR) == status[r])
			continue;

		/* has discipline, ask its opinion */
		if((d = f->disc) && d->exceptf)
		{	if((m = (*d->exceptf)(f,SF_DPOLL,&tm,d)) < 0)
				continue;
			else if(m > 0)
			{	status[r] = m&SF_RDWR;
				continue;
			}
		}

		if(f->extent < 0) /* unseekable stream, must poll/select */
			check[c++] = r;
		else /* seekable streams are always ready */
		{	if(f->flags&SF_READ)
				status[r] |= SF_READ;
			if(f->flags&SF_WRITE)
				status[r] |= SF_WRITE;
		}
	}

	np = -1;
#if _lib_poll
	if(c > 0)
	{	struct pollfd*	fds;

		/* construct the poll array */
		for(m = 0, r = 0; r < c; ++r, ++m)
		{	f = fa[check[r]];
			if(CHKMAINFD(f) && CHKAUXFD(f))
				m += 1;
		}
		if(!(fds = (struct pollfd*)malloc(m*sizeof(struct pollfd))) )
			return -1;

		for(m = 0, r = 0; r < c; ++r, ++m)
		{	f = fa[check[r]];

			if(CHKAUXFD(f) )
			{	fds[m].fd = f->proc->file;
				fds[m].events  = POLLIN;
				fds[m].revents = 0;
				if(!CHKMAINFD(f))
					continue;
				else	m += 1;
			}

			fds[m].fd = f->file;
			fds[m].events = fds[m].revents = 0;
			if((f->flags&SF_READ)  && POLLRD(f) )
				fds[m].events |= POLLIN;
			if((f->flags&SF_WRITE) && POLLWR(f) )
				fds[m].events |= POLLOUT;
		}

		while((np = SFPOLL(fds,m,tm)) < 0 )
		{	if(errno == EINTR || errno == EAGAIN)
				errno = 0;
			else	break;
		}
		if(np > 0) /* poll succeeded */
			np = c;

		for(m = 0, r = 0; r < np; ++r, ++m)
		{	f = fa[check[r]];

			if(CHKAUXFD(f))
			{	if(fds[m].revents & POLLIN)
					status[check[r]] |= SF_READ;
				if(!CHKMAINFD(f))
					continue;
				else	m += 1;
			}

			if((f->flags&SF_READ)  && POLLRD(f) && (fds[m].revents&POLLIN) )
				status[check[r]] |= SF_READ;
			if((f->flags&SF_WRITE) && POLLWR(f) && (fds[m].revents&POLLOUT))
				status[check[r]] |= SF_WRITE;
		}

		free((Void_t*)fds);
	}
#endif /*_lib_poll*/

#if _lib_select
	if(np < 0 && c > 0)
	{	fd_set		rd, wr;
		struct timeval	tmb, *tmp;

		FD_ZERO(&rd);
		FD_ZERO(&wr);
		m = 0;
		for(r = 0; r < c; ++r)
		{	f = fa[check[r]];

			if(CHKAUXFD(f) )
			{	if(f->proc->file > m)
					m = f->proc->file;
				FD_SET(f->proc->file, &rd);
				if(!CHKMAINFD(f))
					continue;
			}

			if(f->file > m)
				m = f->file;
			if((f->flags&SF_READ)  && POLLRD(f))
				FD_SET(f->file,&rd);
			if((f->flags&SF_WRITE) && POLLWR(f))
				FD_SET(f->file,&wr);
		}
		if(tm < 0)
			tmp = NIL(struct timeval*);
		else
		{	tmp = &tmb;
			tmb.tv_sec = tm/SECOND;
			tmb.tv_usec = (tm%SECOND)*SECOND;
		}

		while((np = select(m+1,&rd,&wr,NIL(fd_set*),tmp)) < 0 )
		{	if(errno == EINTR)
				errno = 0;
			else	break;
		}
		if(np > 0)
			np = c;

		for(r = 0; r < np; ++r)
		{	f = fa[check[r]];
			if(CHKAUXFD(f))
			{	if(FD_ISSET(f->proc->file, &rd))
					status[check[r]] |= SF_READ;
				if(!CHKMAINFD(f))
					continue;
			}

			if(((f->flags&SF_READ)  && POLLRD(f) && FD_ISSET(f->file,&rd)) )
				status[check[r]] |= SF_READ;
			if(((f->flags&SF_WRITE) && POLLWR(f) && FD_ISSET(f->file,&wr)) )
				status[check[r]] |= SF_WRITE;
		}
	}
#endif /*_lib_select*/

	/* announce readiness */
	for(c = 0; c < n; ++c)
		if(status[c] != 0 && (d = fa[c]->disc) && d->exceptf)
			(*d->exceptf)(fa[c],SF_READY,(Void_t*)status[c],d);

	/* move ready streams to the front */
	for(r = c = 0; c < n; ++c)
	{	if(status[c] == 0)
			continue;
		fa[c]->val = (ssize_t)status[c];
		if(c > r)
		{	f = fa[r];
			fa[r] = fa[c];
			fa[c] = f;
		}
		r += 1;
	}

	free((Void_t*)status);
	return r;
}
