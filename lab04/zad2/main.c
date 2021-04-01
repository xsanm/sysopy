#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define T_SIGNAL SIGUSR1

void handler_info(int sig, siginfo_t *info, void *ucontext) {
    printf("Signal handled, SIG %d, PID %d, PPID %d\n", sig, getpid(), getppid());
    puts("part of siginfo struct");
    printf("Signal number %d\n", info->si_signo);
    printf("Signal code %d\n", info->si_code);
    printf("Sending process ID %d\n", info->si_pid);
    printf("Real user ID of sending process %d\n", info->si_uid);
    printf("User time consumed %d\n", (int)info->si_utime);
    printf("System time consumed %d\n", (int)info->si_stime);

    return;
}

void handler_child_state_change(int sig) {
    printf("Child state change handler SIG %d PID %d, PPID %d\n", sig, getpid(), getppid());

}






void case_1() {
    puts("\n----------SA_SIGINFO----------");
    struct sigaction sig;
    sig.sa_sigaction = &handler_info;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sig, NULL);

    kill(getpid(), SIGUSR1);
}
void case_2() {
    puts("\n----------SA_NOCLDSTOP----------");
    struct sigaction sig;
    sig.sa_handler = &handler_child_state_change;

    sigaction(SIGCHLD, &sig, NULL);

    pid_t child_id;
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGSTOP to child after 1s");
        sleep(1);
        kill(child_id, SIGSTOP);
        //wait(NULL);
    }

    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child after 1s");
        sleep(1);
        kill(child_id, SIGKILL);
        wait(NULL);
    }

    puts("\nSA_NOCLDSTOP is set up\n");

    sig.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sig, NULL);
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGSTOP to child after 1s");
        sleep(1);
        kill(child_id, SIGSTOP);
        //wait(NULL);
    }

    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child after 1s");
        sleep(1);
        kill(child_id, SIGKILL);
        wait(NULL);
    }
    //STOPPED PROCESSES ARENT KILLED
    puts("\n\nHandler to SIGCHLD wasn't call when SIGSTOP was sended second time");
}

void case_3() {
    puts("\n----------SA_RESETHAND----------");
    struct sigaction sig;
    sig.sa_handler = &handler_child_state_change;

    sigaction(SIGCHLD, &sig, NULL);

    pid_t child_id;
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child");
        kill(child_id, SIGKILL);
        wait(NULL);
    }
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child");
        kill(child_id, SIGKILL);
        wait(NULL);
    }
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child");
        kill(child_id, SIGKILL);
        wait(NULL);
    }

    puts("\nSA_RESETHAND is set up\n");

    sig.sa_flags = SA_RESETHAND;
    sigaction(SIGCHLD, &sig, NULL);
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child");
        kill(child_id, SIGKILL);
        wait(NULL);
    }
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child");
        kill(child_id, SIGKILL);
        wait(NULL);
    }
    if((child_id = fork()) == 0) {
        for(;;);
    } else {
        puts("Sending SIGKILL to child");
        kill(child_id, SIGKILL);
        wait(NULL);
    }

    puts("\n\nHandler ran only once, after first call there was default action (IGNORE) restored");

}

int main(void) {
    
    case_1();

    case_2();

    case_3();


    return 0;
}