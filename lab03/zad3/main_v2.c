#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv) {

    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    int pocess_number = strtol(argv[1], NULL, 10);


    pid_t child_pid = getpid();
    printf("Parent-process pid: %d\n\n", getpid());

    for(int id = 0; id <= pocess_number; id++) {
        if(child_pid == 0) {
            printf("Child-process nr: %d, pid: %d, ppid %d\n", id, getpid(), getppid());
            return 0;
        } else {
            child_pid = fork();
        }
    }

    return 0;
}



