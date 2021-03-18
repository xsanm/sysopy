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

const size_t BUFF_SIZE = 1024;

double calculate_time_clocks(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC;
}

double calculate_time_tics(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void merge_sys(const char *file_name_1, const char *file_name_2) {
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
            write(1, &buff1[ptr1], 1);
            ptr1++;
            act_row_1 = buff1[ptr1 - 1] != '\n';
            if(ptr1 >= readed_1) {
                readed_1 = read(f1, buff1, BUFF_SIZE);
                ptr1 = 0;
            }
        }

        bool act_row_2 = true;
        while(readed_2 != 0 && act_row_2) {
            write(1, &buff2[ptr2], 1);
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
}

void merge_lib(const char *file_name_1, const  char *file_name_2) {
    //printf("Files: %s %s\n", file_name_1, file_name_2);
    FILE *f1 = fopen(file_name_1, "r");
    FILE *f2 = fopen(file_name_2, "r");
    if(f1 == NULL || f2 == NULL) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("LIBRARY");


    char *buff1 = malloc(BUFF_SIZE * sizeof (char ));
    char *buff2 = malloc(BUFF_SIZE * sizeof (char ));

    size_t readed_1 = fread(buff1, sizeof (char), BUFF_SIZE, f1);
    size_t readed_2 = fread(buff2, sizeof (char), BUFF_SIZE, f2);
    size_t ptr1 = 0, ptr2 = 0;

    while(readed_1 || readed_2) {
        bool act_row_1 = true;
        while(readed_1 != 0 && act_row_1) {
            fwrite(&buff1[ptr1], sizeof (char), 1, stdout);
            ptr1++;
            act_row_1 = buff1[ptr1 - 1] != '\n';
            if(ptr1 >= readed_1) {
                readed_1 = fread(buff1, sizeof (char), BUFF_SIZE, f1);
                ptr1 = 0;
            }
        }

        bool act_row_2 = true;
        while(readed_2 != 0 && act_row_2) {
            fwrite(&buff2[ptr2], sizeof (char), 1, stdout);
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

    struct tms operation_time[5]; //usr and sys
    clock_t operation_time_real[5]; //real

    times(&operation_time[0]);
    operation_time_real[0] = clock();

    merge_lib(file_name_1, file_name_2);

    times(&operation_time[1]);
    operation_time_real[1] = clock();

    merge_sys(file_name_1, file_name_2);

    times(&operation_time[2]);
    operation_time_real[2] = clock();

    puts("\n              REAl        USER        SYSTEM");
    printf("LIBRARY     %lf", calculate_time_clocks(operation_time_real[0], operation_time_real[1]));
    printf("    %lf", calculate_time_tics(operation_time[0].tms_utime, operation_time[1].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[0].tms_stime, operation_time[1].tms_stime));

    printf("SYSTEM      %lf", calculate_time_clocks(operation_time_real[1], operation_time_real[2]));
    printf("    %lf", calculate_time_tics(operation_time[1].tms_utime, operation_time[2].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[1].tms_stime, operation_time[2].tms_stime));

    free(file_name_1);
    free(file_name_2);

    return 0;
}