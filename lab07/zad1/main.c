#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>



void run_pizzeria(int cooks_number, int supplier_number) {
    for (int i = 0; i < cooks_number; i++) {
        pid_t child_pid = fork();
        char snum[5];
        sprintf(snum, "%d", i + 1);
        if (child_pid == 0) {
            execlp("./cook",  "./cook", snum, NULL);
        }
    }

    for (int i = 0; i < supplier_number; i++) {
        pid_t child_pid = fork();
        char snum[5];
        sprintf(snum, "%d", i + 1);
        if (child_pid == 0) {
            execlp("./supplier",  "./supplier", snum, NULL);
        }
    }
    for(int i = 0; i < cooks_number + supplier_number; i++) {
        wait(NULL);
    }
}


int main(int argc, char **argv) {
    if(argc != 3) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    int N = strtol (argv[1], NULL, 10);
    int M = strtol (argv[2], NULL, 10);
    printf("STARTING PIZZERIA with %d cooks and %d suppliers\n", N, M);

    run_pizzeria(N, M);

    return 0;
}