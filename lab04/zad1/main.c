#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define T_SIGNAL SIGUSR1


void handler_parent(int sig) {
    printf("Signal handled in parent, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    return;
}
void handler_child(int sig) {
    printf("Signal handled in child, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    return;
}

void handler(int sig) {
    printf("Signal handled, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    return;
}

void test_ignore() {
    puts("\n-----Testing ingore-----");
    struct sigaction sig;
    sig.sa_handler = SIG_IGN;
    sigaction(T_SIGNAL, &sig, NULL);

    pid_t child_id;
    if((child_id = fork()) < 0) {
        perror("Fork fucked");
        exit(1);
    }
    if(child_id != 0) {
        puts("Raising in parent");
        raise(T_SIGNAL);
        puts("Not terminated, so signal ingored in parent");
        wait(NULL);
    } else {
        puts("Raising in child");
        raise(T_SIGNAL);
        puts("Not terminated, so signal ingored in child");
        wait(NULL);
    }
}

void test_handler() {
    puts("\n-----Testing handler-----");
    struct sigaction sig;
    sig.sa_handler = &handler;
    sigaction(T_SIGNAL, &sig, NULL);

    pid_t child_id;
    if((child_id = fork()) < 0) {
        perror("Fork fucked");
        exit(1);
    }
    if(child_id != 0) {
        puts("Raising in parent");
        raise(T_SIGNAL);
        wait(NULL);
    } else {
        puts("Raising in child");
        raise(T_SIGNAL);

    }
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

    pid_t child_id;
    if((child_id = fork()) < 0) {
        perror("Fork fucked");
        exit(1);
    }
    if(child_id != 0) {
        sleep(1); //sleep to not get lost in outputs
        puts("Raising in parent");
        raise(T_SIGNAL);

        sigset_t waiting_mask;
        sigpending(&waiting_mask);
        if(sigismember(&waiting_mask, T_SIGNAL)) {
            puts("Signal masked, nothing happend in parent");
        } else {
            puts("Signal not masked, so if you see this signal wasnt raised");
        }

        struct sigaction sig; //creating handler
        sig.sa_handler = &handler_parent;
        sigaction(T_SIGNAL, &sig, NULL);

        puts("Unmasking signal and handling");
        sigemptyset(&new_mask);
        sigaddset(&new_mask, T_SIGNAL);
        sigprocmask(SIG_UNBLOCK, &new_mask, &old_mask);
        wait(NULL);
    } else {
        puts("Raising in child");

        raise(T_SIGNAL);

        sigset_t waiting_mask;
        sigpending(&waiting_mask);

        if(sigismember(&waiting_mask, T_SIGNAL)) {
            puts("Signal masked, nothing happend in child");
        } else {
            puts("Signal not masked, so if you see this signal wasnt raised");
        }

        struct sigaction sig; //creating handler
        sig.sa_handler = &handler_child;
        sigaction(T_SIGNAL, &sig, NULL);

        puts("Unmasking signal and handling");
        sigemptyset(&new_mask);
        sigaddset(&new_mask, T_SIGNAL);
        sigprocmask(SIG_UNBLOCK, &new_mask, &old_mask);
    }
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

    pid_t child_id;
    if((child_id = fork()) < 0) {
        perror("Fork fucked");
        exit(1);
    }
    if(child_id != 0) {
        printf("%s\n", sigismember (&waiting_mask, T_SIGNAL) ? "Pending in parent" : "Not Pendingin parent");
    } else {
        printf("%s\n", sigismember (&waiting_mask, T_SIGNAL) ? "Pending in parent" : "Not Pendingin parent");
    }

}


int main(int argc, char **argv) {

    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    if(strcmp(argv[1], "ignore") == 0) {
        test_ignore();
    } else if(strcmp(argv[1], "handler") == 0) {
        test_handler();
    } else if(strcmp(argv[1], "mask") == 0) {
        test_mask();
    } else if(strcmp(argv[1], "pending") == 0) {
        test_pending();
    } else {
        puts("WRONG ARGUMENT");
    }

    return 0;
}

