#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


static int PROCESS_CNT;

void make_process(int id, int process_number) {
    if(PROCESS_CNT >= process_number) return;
    pid_t child_pid = fork();

    PROCESS_CNT++;
    if(child_pid == 0) {
        printf("Child-process nr: %d, pid: %d, ppid %d\n", id, getpid(), getppid());
    } else {
        make_process(id + 1, process_number);
    }
}


int main(int argc, int **argv) {
    PROCESS_CNT = 0;

    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    int pocess_number = strtol((const char*)argv[1], NULL, 10);

    printf("Parent-process pid: %d\n\n", getpid());
    make_process(1, pocess_number);

    return 0;
}



