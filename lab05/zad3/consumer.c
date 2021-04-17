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

const int BUFF_FILL = 1024;


int main(int argc, char **argv) {
    puts("Consumer");
    if(argc != 4) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }


    char *fifo_name = argv[1];
    char *file_name = argv[2];
    int n =  strtol(argv[3], NULL, 10);

    FILE *file_d = fopen(file_name, "w+");
    if(file_d < 0) {
        puts("ERROR WHILE OPENING FILE");
        return 2;
    }

    FILE *fifo_d = fopen(fifo_name, "r");
    if(fifo_d < 0) {
        puts("ERROR WHILE OPENING FIFO");
        return 2;
    }

    char fill[BUFF_FILL];
    for(int i = 0; i < BUFF_FILL - 1; i++) {
        fill[i] = ' ';
    }
    fill[BUFF_FILL - 1] = '\n';

    int *line_chars = NULL;
    int lines = 0;

    char *data = malloc(sizeof (char) * n);
    int line;
    while(fread(&line, sizeof(int), 1, fifo_d)) {
        fread(data, sizeof (char), n, fifo_d);

        if(line > lines) {
            printf("%d\n", line);
            line_chars = realloc(line_chars, line + 1);
            for(int i = lines + 1; i <= line; i++){
                line_chars[i] = 4;
                fseek(file_d, BUFF_FILL * (i - 1), SEEK_SET);

                char str[5];
                sprintf(str, "%d", i);

                fwrite(fill, sizeof (char ), strlen(fill), file_d);
                fseek(file_d, BUFF_FILL * (i - 1), SEEK_SET);
                fwrite(str, sizeof (char ), strlen(str), file_d);
            }
            lines = line;
        }

        fseek(file_d, BUFF_FILL * (line - 1) + line_chars[line], SEEK_SET);
        fwrite(data, sizeof (char), n, file_d);
        line_chars[line] += n;

        printf("%d %s\n", line, data);
    }

    fclose(file_d);
    fclose(fifo_d);

    return 0;
}