#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define T_SIGNAL SIGUSR1

void handler(int sig) {
    printf("Signal handled in exec, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    return;
}

void test_ignore() {
    //puts("\n-----Testing ingore-----");
    puts("Raising in exec");
    raise(T_SIGNAL);
    puts("Not terminated, so signal ingored in executed process");
}


void test_mask() {
    //puts("\n-----Testing mask-----");
    sigset_t new_mask;
    sigset_t old_mask;
    sigemptyset(&new_mask);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    if(sigismember(&old_mask, T_SIGNAL)) {
        printf("%d Blocked in exec\n", T_SIGNAL);
    } else {
        printf("%d Not blocked in exec\n", T_SIGNAL);
    }

    puts("Raising in exec");

    raise(T_SIGNAL);

    sigset_t waiting_mask;
    sigpending(&waiting_mask);

    if(sigismember(&waiting_mask, T_SIGNAL)) {
        puts("Signal masked, nothing happend in child");
    } else {
        puts("Signal not masked, so if you see this signal wasnt raised");
    }

    struct sigaction sig; //creating handler
    sig.sa_handler = &handler;
    sigaction(T_SIGNAL, &sig, NULL);

    puts("Unmasking signal and handling");
    sigemptyset(&new_mask);
    sigaddset(&new_mask, T_SIGNAL);
    sigprocmask(SIG_UNBLOCK, &new_mask, &old_mask);
}

void test_pending() {
   // puts("\n-----Testing pending-----");
    sigset_t waiting_mask;
    sigpending (&waiting_mask);
    printf("%s\n", sigismember (&waiting_mask, T_SIGNAL) ? "Pending in exec" : "Not Pending in exec");
}


int main(int argc, char **argv) {
    puts("EXECUTED CODE BELOW");
    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }


    if(strcmp(argv[1], "ignore") == 0) {
        test_ignore();
    } else if(strcmp(argv[1], "mask") == 0) {
        test_mask();
    } else if(strcmp(argv[1], "pending") == 0) {
        test_pending();
    } else {
        puts("WRONG ARGUMENT");
    }
    puts("END OF EXECUTED CODE");
    return 0;
}

