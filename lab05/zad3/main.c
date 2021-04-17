#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


int main() {
    if(mkfifo("fifo_1", 0777) == -1) {
        if(errno != EEXIST) {
            puts("ERROR WHILE CREATING FIFO");
            return 1;
        }
    }
    //./producer fifo_1 4 a.txt 5
    char *to_exec0[] = {"./consumer", "fifo_1", "b.txt",  "5", NULL};

    char *to_exec1[] = {"./producer", "fifo_1", "1", "a1.txt", "5", NULL};
    char *to_exec2[] = {"./producer", "fifo_1", "3", "a2.txt", "2", NULL};
    char *to_exec3[] = {"./producer", "fifo_1", "5", "a3.txt", "5", NULL};
    char *to_exec4[] = {"./producer", "fifo_1", "2", "a4.txt", "1", NULL};
    char *to_exec5[] = {"./producer", "fifo_1", "7", "a5.txt", "10", NULL};
    //char *to_exec1[] = {"producer", "fifo_1", "1", "a.txt", "5", NULL};
    if(fork() == 0) execvp(to_exec1[0], to_exec1);
    if(fork() == 0)  execvp(to_exec2[0], to_exec2);
    if(fork() == 0)  execvp(to_exec3[0], to_exec3);
    if(fork() == 0)  execvp(to_exec4[0], to_exec4);
    if(fork() == 0)  execvp(to_exec5[0], to_exec5);
    if(fork() == 0)  execvp(to_exec0[0], to_exec0);

    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);
    return 0;
}