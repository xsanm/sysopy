#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>


int main(int argc, char **argv) {
    if(argc != 5) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    char *fifo_name = argv[1];
    int  row = strtol(argv[2], NULL, 10);
    char *file_name = argv[3];
    int n =  strtol(argv[4], NULL, 10);

    FILE *file_d = fopen(file_name, "r");
    if(file_d < 0) {
        puts("ERROR WHILE OPENING FILE");
        return 2;
    }

    FILE *fifo_d = fopen(fifo_name, "w");
    if(fifo_d < 0) {
        puts("ERROR WHILE OPENING FIFO");
        return 2;
    }

    char *data = malloc(sizeof (char) * n);
    while(fread(data, sizeof (char), n, file_d) == n) {
        //sleep(1);
        fwrite(&row,sizeof(int),1, fifo_d);
        fwrite(data, sizeof (char), n, fifo_d);
    }

    fclose(file_d);
    fclose(fifo_d);


    return 0;
}