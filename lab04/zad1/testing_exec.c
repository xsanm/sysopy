#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define T_SIGNAL SIGUSR1


void handler(int sig) {
    printf("Signal handled int main, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    return;
}

void test_ignore() {
    puts("\n-----Testing ingore-----");

    struct sigaction sig;
    sig.sa_handler = SIG_IGN;
    sigaction(T_SIGNAL, &sig, NULL);


    puts("Raising in main program");
    raise(T_SIGNAL);
    puts("Not terminated, so signal ingored in main program");

    char *args[] = { "./to_exec", "ignore", NULL };
    execvp(args[0],args);
}

void test_mask() {
    puts("\n-----Testing mask-----");
    sigset_t new_mask;
    sigset_t old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, T_SIGNAL);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

    if(sigismember(&new_mask, T_SIGNAL)) {
        printf("%d Blocked\n", T_SIGNAL);
    } else {
        printf("%d Not blocked\n", T_SIGNAL);
    }

    puts("Raising in main");
    raise(T_SIGNAL);

    sigset_t waiting_mask;
    sigpending(&waiting_mask);
    if(sigismember(&waiting_mask, T_SIGNAL)) {
        puts("Signal masked, nothing happend in main");
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

    //masking once again coz singal was unblocked
    sigemptyset(&new_mask);
    sigaddset(&new_mask, T_SIGNAL);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);


    char *args[] = { "./to_exec", "mask", NULL };
    execvp(args[0],args);

}

void test_pending() {
    puts("\n-----Testing pending-----");

    sigset_t base_mask;
    sigset_t waiting_mask;

    sigemptyset (&base_mask);
    sigaddset (&base_mask, T_SIGNAL);
    sigprocmask(SIG_SETMASK, &base_mask, NULL);

    raise(T_SIGNAL); //if comment should not pending

    sigpending (&waiting_mask);
    printf("%s\n", sigismember (&waiting_mask, T_SIGNAL) ? "Pending in main" : "Not Pendingin main");

    char *args[] = { "./to_exec", "pending", NULL };
    execvp(args[0],args);

}


int main(int argc, char **argv) {

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

    return 0;
}

