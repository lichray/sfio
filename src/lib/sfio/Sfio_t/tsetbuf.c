#include	"sftest.h"

Sfdisc_t Disc;

#if __STD_C
main(void)
#else
main()
#endif
{
	int	n, fd;
	Sfio_t	*f;
	char	*s, buf[1024];
	int	fdv[100];

	buf[0] = 0;
	sfsetbuf(sfstdout,buf,sizeof(buf));
	if(!sfstdout->pool)
		terror("No pool\n");
	sfdisc(sfstdout,&Disc);
	sfset(sfstdout,SF_SHARE,0);
	sfputr(sfstdout,"123456789",0);
	if(strcmp(buf,"123456789") != 0)
		terror("Setting own buffer for stdout\n");
	if(sfpurge(sfstdout) < 0)
		terror("Purging sfstdout\n");

	if((fd = creat("xxx",0666)) < 0)
		terror("Creating xxx\n");

	if(write(fd,buf,sizeof(buf)) != sizeof(buf))
		terror("Writing to xxx\n");
	if(lseek(fd,0L,0) < 0)
		terror("Seeking back to origin\n");

	if(!(f = sfnew((Sfio_t*)0,buf,sizeof(buf),fd,SF_WRITE)))
		terror("Making stream\n");

	if(!(s = sfreserve(f,-1,1)) || s != buf)
		terror("sfreserve1 returns the wrong pointer\n");
	sfwrite(f,s,0);

#define NEXTFD	12
	if((fd+NEXTFD) < (sizeof(fdv)/sizeof(fdv[0])) )
	{	struct stat	st;
		int		i;
		for(i = 0; i < fd+NEXTFD; ++i)
			fdv[i] = fstat(i,&st);
	}
	if((n = sfsetfd(f,fd+NEXTFD)) != fd+NEXTFD)
		terror("Try to set file descriptor to %d but get %d\n",fd+NEXTFD,n);
	if((fd+NEXTFD) < (sizeof(fdv)/sizeof(fdv[0])) )
	{	struct stat	st;
		int		i;
		for(i = 0; i < fd+NEXTFD; ++i)
			if(i != fd && fdv[i] != fstat(i,&st))
				terror("Fd %d changes status after sfsetfd %d->%d\n",
					i, fd, fd+NEXTFD);
	}

	if(!(s = sfreserve(f,-1,1)) || s != buf)
		terror("sfreserve2 returns the wrong pointer\n");
	sfwrite(f,s,0);

	if(sfsetbuf(f,(char*)0,-1) != buf)
		terror("sfsetbuf didnot returns last buffer\n");

	sfsetbuf(f,buf,sizeof(buf));

	if(sfreserve(f,-1,1) != buf || sfslen() != sizeof(buf) )
		terror("sfreserve3 returns the wrong value\n");
	sfwrite(f,s,0);

	system("rm xxx >/dev/null 2>&1");
	return 0;
}
