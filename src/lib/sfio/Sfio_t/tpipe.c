#include	"sftest.h"
#include	<signal.h>

#if __STD_C
void handler(int sig)
#else
handler(sig)
int sig;
#endif
{
	sfprintf(sfstdout,"ok 1");
}

#if __STD_C
void alrmf(int sig)
#else
void alrmf(sig)
int	sig;
#endif
{
	terror("Alarm went off\n");
}

main()
{
	int	fd[2];
	Sfio_t	*fr, *f2, *fw;
	char	*s;
	int	i, j, n;
	char	buf[1024];

	if(sfnew(sfstdout,buf,sizeof(buf),-1,SF_STRING|SF_WRITE) != sfstdout)
		terror("Reopen sfstdout\n");

	if(pipe(fd) < 0)
		terror("Can't open pipe\n");
	if(!(fr = sfnew(NIL(Sfio_t*),NIL(char*),-1,fd[0],SF_READ)) ||
	   !(fw = sfnew(NIL(Sfio_t*),NIL(char*),-1,fd[1],SF_WRITE)) )
		terror("Can't open stream\n");
	signal(SIGALRM,alrmf);
	sfwrite(fw,"0123456789",10);
	alarm(4);
	if(sfread(fr,buf,10) != 10)
		terror("Can't read data from pipe\n");
	sfwrite(fw,"0123456789",10);
	if(sfmove(fr,fw,10L,-1) != 10)
		terror("sfmove failed\n");
	alarm(0);
	sfpurge(fw);
	sfclose(fw);
	sfpurge(fr);
	sfclose(fr);

	if(pipe(fd) < 0)
		terror("Can't open pipe2\n");
	if(!(fr = sfnew(NIL(Sfio_t*),NIL(char*),-1,fd[0],SF_READ)) ||
	   !(f2 = sfnew(NIL(Sfio_t*),NIL(char*),-1,fd[0],SF_READ)) ||
	   !(fw = sfnew(NIL(Sfio_t*),NIL(char*),-1,fd[1],SF_WRITE)) )
		terror("Can't open stream\n");
	sfset(fr,SF_SHARE|SF_LINE,1);
	sfset(f2,SF_SHARE|SF_LINE,1);
	sfset(fw,SF_SHARE,1);

	if(sfwrite(fw,"1\n2\n3\n",6) != 6)
		terror("sfwrite failed0\n");
	i = j = -1;
	if(sfscanf(fr,"%d%d\n%n",&i,&j,&n) != 2 || i != 1 || j != 2 || n != 4)
		terror("sfscanf failed0\n");
	
	if(sfscanf(f2,"%d\n%n",&i,&n) != 1 || i != 3 || n != 2)
		terror("sfscanf failed on co-stream\n");

	if(sfwrite(fw,"123\n",4) != 4)
		terror("sfwrite failed\n");
	if(!(s = sfreserve(fr,4,0)) )
		terror("sfreserve failed\n");

	sfputr(fw,"abc",'\n');
	if(sfmove(fr,fw,1,'\n') != 1)
		terror("sfmove failed\n");
	if(!(s = sfgetr(fr,'\n',1)) || strcmp(s,"abc") != 0)
		terror("sfgetr failed\n");

	if(sfwrite(fw,"111\n222\n333\n444\n",16) != 16)
		terror("Bad write to pipe\n");

	if(!(s = sfgetr(fr,'\n',1)) )
		terror("sfgetr failed\n");
	if(strcmp(s,"111") != 0)
		terror("sfgetr got wrong string\n");
	if(!(s = sfreserve(fr,0,1)) || sfslen() != 0)
		terror("sfgetr read too much\n");
	sfread(fr,s,0);

	if(sfmove(fr,sfstdout,2,'\n') != 2)
		terror("sfmove failed2\n");
	sfputc(sfstdout,0);
	if(strcmp("222\n333\n",buf) != 0)
		terror("sfmove got wrong data\n");
	if(!(s = sfreserve(fr,0,1)) || sfslen() != 0)
		terror("sfmove read too much\n");
	sfread(fr,s,0);
	if(sfmove(fr,NIL(Sfio_t*),1,'\n') != 1)
		terror("sfmove failed\n");

	if(sfwrite(fw,"0123456789",11) != 11)
		terror("Bad write to pipe2\n");
	if(!(s = sfreserve(fr,11,0)) )
		terror("Bad peek size %d, expect 11\n",sfslen());
	if(strncmp(s,"0123456789",10) != 0)
		terror("Bad peek str %s\n",s);

	buf[0] = 0;
	sfseek(sfstdout,0L,0);

	sfclose(fr);

	signal(SIGPIPE,handler);
	sfprintf(fw,"not ok 1");
	sfpurge(fw);

	sfprintf(sfstdout,"ok 2");
	sfputc(sfstdout,0);

	if(strcmp(buf,"ok 1ok 2") != 0)
		terror("Wrong output\n");

	exit(0);
}
