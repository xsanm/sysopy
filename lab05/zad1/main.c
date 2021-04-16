#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define P_END 0
#define P_BEG 1

int main(int argc, char **argv) {

    int status;
    int i;


    char *args[3][4] = {
            {"cat", "/etc/passwd", NULL},
            {"grep", "^s", NULL},
            {"cut", "-b", "1-20", NULL}
    };

    int fds[4][2];


    for(int i = 0; i < 3; i++) {
        if (pipe(fds[i]) < 0) {
            puts("ERROR can't make a pipe");
            exit(1);
        }
    }

    for(int i = 0; i < 3; i++) {
        if (fork() == 0) {
            //printf("%d\n",i);
            if( i > 0) { //pierwszy nie potrzebuje in
                dup2(fds[i - 1][P_END], STDIN_FILENO);
            }
            if(i < 3 - 1) { //ostatni nie potrzebuje out
                dup2(fds[i][P_BEG], STDOUT_FILENO);
            }
            for(int j = 0; j < 2; j++) { //dup2 duplikuje, czyli zamykam zeby nie czekac w programie na EOF
                close(fds[j][P_BEG]);
                close(fds[j][P_END]);
            }
            execvp(args[i][0], args[i]);
        }
        //printf("%d\n",i);
    }
    //zamykam w rodzicu
    for(int j = 0; j < 3; j++) {
        close(fds[j][P_BEG]);
        close(fds[j][P_END]);
    }
    for (i = 0; i < 3; i++)
        wait(&status);


    return 0;
}