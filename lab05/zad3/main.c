#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

//CASE1 5 producers - 1 consumer
//CASE2 1 producer - 5 consumer
//CASE3 5 producer - 5 consumer

char FIFO_NAME[] = "fifo_1";

char *P_FILES[] = { "a1.txt", "a2.txt", "a3.txt", "a4.txt", "a5.txt" };

char *C_FILES[] = { "b1.txt", "b2.txt", "b3.txt" };

char P_PROG[] = "./producer";
char C_PROG[] = "./consumer";

char N1[] = "5";
char N2[] = "4096";


void case1() {
    puts("5 producers - 1 consumer");
    char *C_EXEC[] = { C_PROG, FIFO_NAME, C_FILES[0], N1 , NULL};
    if(fork() == 0)  execvp(C_EXEC[0], C_EXEC);

    for(int i = 0; i < 5; i++) {
        char str[5];
        sprintf(str, "%d", i + 1);
        char *P_EXEC[] = { P_PROG, FIFO_NAME, str, P_FILES[i], N1 , NULL};
        if(fork() == 0) execvp(P_EXEC[0], P_EXEC);
    }

    for(int i = 0; i < 6; i++) wait(NULL);
}

void case2() {
    puts("1 producer - 5 consumers");
    char *C_EXEC[] = { C_PROG, FIFO_NAME, C_FILES[1], N1 , NULL};
    for(int i = 0; i < 5; i++) {
        if(fork() == 0)  execvp(C_EXEC[0], C_EXEC);
    }

    char *P_EXEC[] = { P_PROG, FIFO_NAME, "2", P_FILES[4], N1 , NULL};
    if(fork() == 0) execvp(P_EXEC[0], P_EXEC);

    for(int i = 0; i < 6; i++) wait(NULL);
}

void case3() {
    puts("5 producers - 5 consumers");
    for(int i = 0; i < 5; i++) {
        char str[5];
        sprintf(str, "%d", i + 1);
        char *P_EXEC[] = { P_PROG, FIFO_NAME, str, P_FILES[i], N1 , NULL};
        if(fork() == 0) execvp(P_EXEC[0], P_EXEC);
    }
    char *C_EXEC[] = { C_PROG, FIFO_NAME, C_FILES[2], N1 , NULL};
    for(int i = 0; i < 5; i++) {
        if(fork() == 0)  execvp(C_EXEC[0], C_EXEC);
    }

    for(int i = 0; i < 10; i++) wait(NULL);
}

int main(int argc, char **argv) {

    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    if(mkfifo(FIFO_NAME, 0777) == -1) {
        if(errno != EEXIST) {
            puts("ERROR WHILE CREATING FIFO");
            return 1;
        }
    }

    if(strcmp(argv[1], "CASE1") == 0) {
        case1();
    } else if(strcmp(argv[1], "CASE2") == 0) {
        case2();
    } else if(strcmp(argv[1], "CASE3") == 0) {
        case3();
    } else {
        puts("WRONG ARGUMENT");
        return 1;
    }


    return 0;
}