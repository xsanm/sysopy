#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int MODE;
int SIG_1;
int SIG_2;

int SIGNALS_CNT;

void counting_handler(int sig) {
    SIGNALS_CNT++;
    //puts("SIEMA");
}

void queue_handler(int sig, siginfo_t *info, void *ucontext) {
    SIGNALS_CNT++;
    printf("Caught signals: %d, catcher index: %d\n", SIGNALS_CNT, info->si_value.sival_int);
}

void ending_handler(int sig){
    printf("Caught signals from catcher %d\n", SIGNALS_CNT);
    exit(0);
}

void mode_kill(pid_t catcher, int no_signals) {
    for(int i = 0; i < no_signals; i++) {
        kill(catcher, SIG_1);
    }
    printf("Singals sent to catcher %d\n", no_signals);
    kill(catcher, SIG_2);
}

void mode_sigqueue(pid_t catcher, int no_signals) {
    union sigval value;
    value.sival_int = 0;
    for (int i = 0; i < no_signals; ++i) {
        sigqueue(catcher, SIG_1, value);
    }
    sigqueue(catcher, SIG_2, value);
}

void mode_sigrt(pid_t catcher, int no_signals) {
    for(int i = 0; i < no_signals; i++) {
        kill(catcher, SIG_1);
    }
    printf("Singals sent to catcher %d\n", no_signals);
    kill(catcher, SIG_2);
}

int main(int argc, char **argv) {
    puts("SENDER");

    if(argc != 4) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    SIGNALS_CNT = 0;

    struct sigaction sig_usr1;
    sig_usr1.sa_handler = counting_handler;
    struct sigaction sig_usr2;
    sig_usr2.sa_handler = ending_handler;


    pid_t catcher = strtol(argv[1], NULL, 10);
    int no_signals = strtol(argv[2], NULL, 10);




    if(strcmp(argv[3], "KILL") == 0) {
        MODE = 1;
        SIG_1 = SIGUSR1;
        SIG_2 = SIGUSR2;
        sigaction(SIG_1, &sig_usr1, NULL);
        sigaction(SIG_2, &sig_usr2, NULL);
        mode_kill(catcher, no_signals);
    } else if(strcmp(argv[3], "SIGQUEUE") == 0) {
        MODE = 2;
        SIG_1 = SIGUSR1;
        SIG_2 = SIGUSR2;

        struct sigaction sig_usr1;
        sig_usr1.sa_sigaction = &queue_handler;
        sig_usr1.sa_flags = SA_SIGINFO;
        sigaction(SIG_2, &sig_usr2, NULL);
        sigaction(SIG_1, &sig_usr1, NULL);

        mode_sigqueue(catcher, no_signals);
    } else if(strcmp(argv[3], "SIGRT") == 0) {
        MODE = 3;
        SIG_1 = SIGRTMIN + 1;
        SIG_2 = SIGRTMIN + 2;
        sigaction(SIG_1, &sig_usr1, NULL);
        sigaction(SIG_2, &sig_usr2, NULL);
        mode_sigrt(catcher, no_signals);
    } else {
        puts("WRONG MODE");
        return 1;
    }


    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIG_1);
    sigdelset(&mask, SIG_2);
    //sigsuspend(&mask);

    for(;;) {
        sigsuspend(&mask);
    }


    return 0;
}