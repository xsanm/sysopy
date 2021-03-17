#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

const size_t BUFF_SIZE = 256;


void merge_lib(char *file_name_1, char *file_name_2) {
    printf("Files: %s %s\n", file_name_1, file_name_2);
    FILE *f1 = fopen(file_name_1, "r");
    FILE *f2 = fopen(file_name_2, "r");
    if(f1 == NULL || f2 == NULL) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("OKAY");


    char *buff1 = malloc(BUFF_SIZE * sizeof (char ));
    char *buff2 = malloc(BUFF_SIZE * sizeof (char ));

    size_t readed_1 = fread(buff1, sizeof (char), BUFF_SIZE, f1);
    size_t readed_2 = fread(buff2, sizeof (char), BUFF_SIZE, f2);
    size_t ptr1 = 0, ptr2 = 0;

    while(readed_1 || readed_2) {
        bool act_row_1 = true;
        while(readed_1 != 0 && act_row_1) {
            putchar(buff1[ptr1]);
            ptr1++;
            act_row_1 = buff1[ptr1 - 1] != '\n';
            if(ptr1 >= readed_1) {
                readed_1 = fread(buff1, sizeof (char), BUFF_SIZE, f1);
                ptr1 = 0;
            }
        }

        bool act_row_2 = true;
        while(readed_2 != 0 && act_row_2) {
            putchar(buff2[ptr2]);
            ptr2++;
            act_row_2 = buff2[ptr2 - 1] != '\n';
            if(ptr2 >= readed_2) {
                readed_2 = fread(buff2, sizeof (char), BUFF_SIZE, f2);
                ptr2 = 0;
            }
        }

    }

    fclose(f1);
    fclose(f2);
    free(buff1);
    free(buff2);
}

int main(int argc, char **argv) {
    puts("start");

    char *file_name_1 = calloc(256, sizeof (char));
    char *file_name_2 = calloc(256, sizeof (char));

    if(argc == 3) {
        strcpy(file_name_1, argv[1]);
        strcpy(file_name_2, argv[2]);
    } else if(argc == 2) {
        strcpy(file_name_1, argv[1]);
        puts("Enter the name of second file, please");
        scanf("%s", file_name_2);
    } else if(argc == 1) {
        puts("Enter the name of first file, please");
        scanf("%s", file_name_1);
        puts("Enter the name of second file, please");
        scanf("%s", file_name_2);
    } else {
        puts("TOO MANY ARGUMENTS");
        return 0;
    }

    merge_lib(file_name_1, file_name_2);

    free(file_name_1);
    free(file_name_2);

    return 0;
}