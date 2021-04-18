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
#include <sys/file.h>

const int BUFF_FILL = 1024;


int main(int argc, char **argv) {
    //puts("Consumer");
    if(argc != 4) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }


    char *fifo_name = argv[1];
    char *file_name = argv[2];
    int n =  strtol(argv[3], NULL, 10);


    FILE *fifo_d = fopen(fifo_name, "r");
    if(fifo_d < 0) {
        puts("ERROR WHILE OPENING FIFO");
        return 2;
    }


    char *data = malloc(sizeof (char) * (n + 1));
    int line;
    while(fread(&line, sizeof(int), 1, fifo_d)) {
        FILE *file_d = fopen(file_name, "rw+");
        if(file_d < 0) {
            puts("ERROR WHILE OPENING FILE");
            return 2;
        }

        fread(data, sizeof (char), n, fifo_d);
        flock(fileno(file_d), LOCK_EX);

        int cnt = 0;
        char c;
        fseek(file_d, 0, SEEK_SET);
        while(fread(&c, sizeof (char), 1, file_d)) {
            if (c == '\n') cnt++;
            if (cnt >= line) {
                fseek(file_d, -1, SEEK_CUR);
                char buff[4096];
                int readed = fread(buff, sizeof(char), 4096, file_d);
                fseek(file_d, -readed, SEEK_CUR);
                fwrite(data, sizeof(char), n, file_d);
                fwrite(buff, sizeof(char), readed, file_d);
                break;
            }

        }
        if(cnt < line) {
            for(int i = cnt + 1; i <= line; i++) {
                char str[5];
                sprintf(str, "%d \n", i);
                fwrite(str, sizeof (char ), strlen(str), file_d);
            }
            fseek(file_d, -1, SEEK_CUR);
            fwrite(data, sizeof(char), n, file_d);
            fwrite("\n", sizeof(char), 1, file_d);
        }

        flock(fileno(file_d), LOCK_UN);
        fclose(file_d);
        //printf("%d %s\n", line, data);
    }


    fclose(fifo_d);

    return 0;
}