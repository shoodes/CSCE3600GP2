//DOUG 
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void myusr1(int sig, siginfo_t * theinfo, void * foo);

pid_t mypid;

int support () {

sigset_t myset;

	// initialsize with sigemptyset(*set) or sigfillset(*set) 
	// examine with sigismember (*set, sig) 
	// change with sigdelset(*set, sig) or sigaddset(*set, sig) 

struct sigaction myaction;

		/*sigaction has atleast:
		sa_handler pointer to function, SIG_DFL or SIG_IGN
		sa_sigaction
		sa_mask a signal mask;
		sa_flags see man page or book
		*/

mypid = getpid();

		sigemptyset(&myset);
		sigaddset(&myset, SIGUSR1);
		

sigemptyset(&myaction.sa_mask);
myaction.sa_sigaction = myusr1;
myaction.sa_flags = SA_SIGINFO;
sigaction(SIGUSR1, &myaction, NULL);
sigemptyset(&myset);
sigaddset(&myset, SIGUSR2);
sigprocmask(SIG_BLOCK,&myset,NULL);
sigemptyset(&myset);
sigprocmask(SIG_BLOCK,NULL,&myset);

		if (1== sigismember(&myset,SIGUSR1)) {
			printf("The SIGUSR1 is blocked\n");
		}

		if (1== sigismember(&myset,SIGUSR2)) {
			printf("The SIGUSR2 is blocked\n");
		}

	printf("All set up in %d \n",mypid);

		while (1) {
			pause();
		}

}

void myusr1(int sig, siginfo_t * theinfo, void * foo) {

sigset_t localset;

printf("SIGUSR1 called\n");

printf("Some more information:\n");

printf(" pid = %d\n", theinfo->si_pid);

printf(" uid = %d\n", theinfo->si_uid);

printf(" signum = %d\n", theinfo->si_signo);

sigpending( &localset);

if (1== sigismember(&localset,SIGUSR2)) {

printf("There is a SIGUSR2 pending \n");

}

}
