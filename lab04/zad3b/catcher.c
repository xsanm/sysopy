#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int SIG_1;
int SIG_2;

int MODE;

int SIGNALS_CNT;
union sigval value;

void counting_handler(int sig, siginfo_t *info, void *ucontext) {
    SIGNALS_CNT++;
    if(MODE != 2) {
        kill(info->si_pid, SIG_1);
    } else {
        sigqueue(info->si_pid, SIG_1, value);
    }

}

void end_handler(int sig, siginfo_t *info, void *ucontext) {
    if(MODE != 2) {
        kill(info->si_pid, SIG_2);
    } else {
        sigqueue(info->si_pid, SIG_2, value);
    }
    printf("Caught signals from SENDER %d\n", SIGNALS_CNT);

    exit(0);
}


int main(int argc, char **argv) {
    puts("CATCHER");


    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    if (strcmp("KILL", argv[1]) == 0) {
        MODE = 1;
        SIG_1 = SIGUSR1;
        SIG_2 = SIGUSR2;
    } else if (strcmp("SIGQUEUE", argv[1]) == 0) {
        MODE = 2;
        SIG_1 = SIGUSR1;
        SIG_2 = SIGUSR2;
    } else if (strcmp("SIGRT", argv[1]) == 0) {
        MODE = 3;
        SIG_1 = SIGRTMIN + 1;
        SIG_2 = SIGRTMIN + 2;
    } else {
        puts("WRONG MODE");
        return 1;
    }

    SIGNALS_CNT = 0;
    value.sival_int = 0;

    struct sigaction sig_usr1;
    sig_usr1.sa_sigaction = counting_handler;
    sig_usr1.sa_flags = SA_SIGINFO;
    sigemptyset(&sig_usr1.sa_mask);
    sigaddset(&sig_usr1.sa_mask, SIG_1);

    sigaction(SIG_1, &sig_usr1, NULL);

    struct sigaction sig_usr2;
    sig_usr2.sa_sigaction = end_handler;
    sig_usr2.sa_flags = SA_SIGINFO;
    sigaction(SIG_2, &sig_usr2, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIG_1);
    sigdelset(&mask, SIG_2);

    printf("%d\n", getpid());
    for(;;) {
        sigsuspend(&mask);
    }
    return 0;
}