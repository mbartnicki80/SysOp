/* 
   proces rodzica tworzy wiele procesow potomnych
   i wykrywa ich za konczenie za pomoca sygnalu
   Kazdy proces pttomny wyswietla komunikat "I'm the child" i "exits" po uplywie n sekund,
   gdzie n "is the sequence in which it was forked" 
   
   Nalezy uzypelnic kod programu, a nastepnie uruchomic program. 
   Komunikaty wyswietlone w termninalu znajduja sie w pliku out.txt
*/

#include <stdlib.h>	
#include <unistd.h>	
#include <signal.h>	
#include <stdio.h>	

#include <sys/types.h>
#include <sys/wait.h>

#define NUMPROCS 4		/* number of processes to fork */
int nprocs;				/* number of child processes   */

int main(int argc, char **argv) {

	void catch(int snum);			/* signal handler */
	void child(int n);			/* the child calls this */
	void parent(int pid);		/* the parent calls this */
	int pid;					/* process ID */
	int i;

	signal(SIGCHLD, catch);							/* detect child termination */

	for (i=0; i < NUMPROCS; i++) {
		switch (pid = fork())
		{
		case 0:			/* a fork returns 0 to the child */
            child(i);
			break;
		case -1:		/* something went wrong */
            perror("fork error");
			return 1;
		default:		/* parent just loops to create more kids */
            nprocs++; //parent(pid);
			break;
		}
	}
	printf("parent: going to sleep\n");

	/* do nothing forever; */
	while ( nprocs != 0) 
	{
		printf("parent: sleeping\n");
		sleep(60);	/* do nothing for a minute */
	}
	printf("parent: exiting\n");
	exit(0);
}

void
child(int n) {
	printf("\tchild[%d]: child pid=%d, sleeping for %d seconds\n", n, getpid(), n);
	sleep(n);							/* do nothing for n seconds */
	printf("\tchild[%d]: I'm exiting\n", n);
	nprocs--;
	exit(100 + n);							/* exit with a return code of 100+n */
}

void
catch(int snum) {
	int pid;
	int status;

	pid = wait(&status);
	printf("parent: child process pid=%d exited with value %d\n", pid, WEXITSTATUS(status));
	nprocs--;
	signal(SIGCHLD, catch);  
}