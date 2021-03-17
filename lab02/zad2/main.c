#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>

const size_t BUFF_SIZE = 256;

double calculate_time_clocks(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC;
}

double calculate_time_tics(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

/*void merge_sys(const char *file_name_1, const char *file_name_2) {
    //printf("Files: %s %s\n", file_name_1, file_name_2);
    int f1 = open(file_name_1, O_RDONLY);
    int f2 = open(file_name_2, O_RDONLY);
    if(f1 == -1 || f2 == -1) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("SYSTEM");


    char *buff1 = malloc(BUFF_SIZE * sizeof (char ));
    char *buff2 = malloc(BUFF_SIZE * sizeof (char ));

    size_t readed_1 = read(f1, buff1, BUFF_SIZE);
    size_t readed_2 = read(f2, buff2, BUFF_SIZE);
    size_t ptr1 = 0, ptr2 = 0;

    while(readed_1 || readed_2) {
        bool act_row_1 = true;
        while(readed_1 != 0 && act_row_1) {
            putchar(buff1[ptr1]);
            ptr1++;
            act_row_1 = buff1[ptr1 - 1] != '\n';
            if(ptr1 >= readed_1) {
                readed_1 = read(f1, buff1, BUFF_SIZE);
                ptr1 = 0;
            }
        }

        bool act_row_2 = true;
        while(readed_2 != 0 && act_row_2) {
            putchar(buff2[ptr2]);
            ptr2++;
            act_row_2 = buff2[ptr2 - 1] != '\n';
            if(ptr2 >= readed_2) {
                readed_2 = read(f2, buff2, BUFF_SIZE);
                ptr2 = 0;
            }
        }

    }

    close(f1);
    close(f2);
    free(buff1);
    free(buff2);
}*/

void merge_lib(const char *file_name, const  char c) {
    printf("File and char: %s %c\n", file_name, c);
    FILE *f = fopen(file_name, "r");
    if(f == NULL) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("LIBRARY");

    char *buff = malloc(BUFF_SIZE * sizeof (char ));
    size_t readed = fread(buff, sizeof (char), BUFF_SIZE, f);
    int ptr = 0;

    while(readed != 0) {
        ptr = 0;
        int c_occur = 0;
        while(buff[ptr] != '\n') {
            if(buff[ptr] == c) c_occur = 1;
            ptr++;
        }
        if(c_occur == 1) printf("%.*s", ptr + 1, buff);
        fseek(f, ptr - readed + 1, 1);
        readed = fread(buff, sizeof (char), BUFF_SIZE, f);
    }

    fclose(f);
    free(buff);
}

int main(int argc, char **argv) {

    char *file_name = calloc(256, sizeof (char));
    char c;

    if(argc == 3 && strlen(argv[1]) == 1) {
        c = *argv[1];
        strcpy(file_name, argv[2]);
    } else {
        puts("WRONG ARGUMENTS");
        return 0;
    }

    struct tms operation_time[5]; //usr and sys
    clock_t operation_time_real[5]; //real

    times(&operation_time[0]);
    operation_time_real[0] = clock();

    merge_lib(file_name, c);

    times(&operation_time[1]);
    operation_time_real[1] = clock();

    //merge_sys(file_name_1, file_name_2);

    times(&operation_time[2]);
    operation_time_real[2] = clock();

    puts("\n              REAl        USER        SYSTEM");
    printf("LIBRARY     %lf", calculate_time_clocks(operation_time_real[0], operation_time_real[1]));
    printf("    %lf", calculate_time_tics(operation_time[0].tms_utime, operation_time[1].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[0].tms_stime, operation_time[1].tms_stime));

    printf("SYSTEM      %lf", calculate_time_clocks(operation_time_real[1], operation_time_real[2]));
    printf("    %lf", calculate_time_tics(operation_time[1].tms_utime, operation_time[2].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[1].tms_stime, operation_time[2].tms_stime));

    free(file_name);


    return 0;
}