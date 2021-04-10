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

void counting_handler(int sig) {
    SIGNALS_CNT++;
    //puts("SIEMA");
}

void send_signals(pid_t sender_pid, siginfo_t *info) {
    if(MODE != 2) {
        for(int i = 0; i < SIGNALS_CNT; i++) {
            kill(sender_pid, SIG_1);
        }
        kill(sender_pid, SIG_2);
    } else {
        union sigval value;
        for (int i = 0; i < SIGNALS_CNT; ++i) {
            value.sival_int = i;
            sigqueue(sender_pid, SIG_1, value);
        }
        //sleep(1);
        sigqueue(sender_pid, SIG_2, value);
    }

    exit(0);
}


void info_handler(int sig, siginfo_t *info, void *ucontext) {
    printf("Caught signals from SENDER %d\n", SIGNALS_CNT);
    send_signals(info->si_pid, info);
    return;
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
    printf("%d\n", getpid());

    struct sigaction sig_usr1;
    sig_usr1.sa_handler = counting_handler;
    sigaction(SIG_1, &sig_usr1, NULL);

    struct sigaction sig_usr2;
    sig_usr2.sa_sigaction = &info_handler;
    sig_usr2.sa_flags = SA_SIGINFO;
    sigaction(SIG_2, &sig_usr2, NULL);

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