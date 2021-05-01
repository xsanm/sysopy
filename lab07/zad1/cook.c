#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


int main(int argc, char **argv) {
    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    int ID = strtol (argv[1], NULL, 10);
    printf("COOK [%d] hired, PID [%d]\n", ID, getpid());


    return 0;
}