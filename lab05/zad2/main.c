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


void print_ordered(char* mode) {
    printf("Print emails ordered by: %s\n", mode);

    char* command;

    if (strcmp(mode,"date") == 0) {
        command = "echo | mail -H";
    } else if (strcmp(mode, "sender") == 0) {
        command = "echo | mail -H | sort -k 3";
    } else {
        puts("WRONG MODE");
        exit(1);
    }

    FILE* file = popen(command, "r");

    if (file == NULL){
        printf("ERROR while opening popen");
        exit(1);
    }

    char *line;
    size_t len = 0, read;
    while((read = getline(&line, &len, file)) != -1) {
        printf("%s", line);
    }
    free(line);
}

void send_email(char* address, char* topic, char* content) {
    puts("Send email\n");

    char *command = malloc(sizeof address + sizeof topic + sizeof content + sizeof (char ) * 32);

    sprintf(command, "echo %s | mail %s -s %s", content, address, topic);
    printf("%s\n", command);

    FILE* file = popen(command, "r");

    if (file == NULL){
        puts("ERROR while opening popen");
        exit(1);
    }
    free(command);
}

int main(int argc, char **argv) {
    if (argc != 2 && argc != 4){
        printf("ERROR wrong number of arguments!");
        exit(1);
    }

    if (argc == 2){
        print_ordered(argv[1]);
    } else {
        send_email(argv[1], argv[2], argv[3]);
    }

    return 0;
}