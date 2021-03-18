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

const size_t BUFF_SIZE = 512;

double calculate_time_clocks(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC;
}

double calculate_time_tics(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void change_sys(const char *file_name_1, const char *file_name_2, const char *n1, const char *n2) {
    //printf("Files: %s %s\n", file_name_1, file_name_2);
    int f1 = open(file_name_1, O_RDONLY);
    int f2 = open(file_name_2, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(f1 == -1 || f2 == -1) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("SYSTEM");
    char cr;
    while(read(f1, &cr, 1) != 0) {
        if(cr != n1[0]) {
            write(f2, &cr, 1);
        } else {
            size_t id = 1;
            while(id < strlen(n1) && read(f1, &cr, 1) != 0) {
                if(cr != n1[id]) break;
                id++;
            }
            if(id == strlen(n1)) {
                write(f2, n2, strlen(n2));
            } else {
                write(f2, n1, id);
                write(f2, &cr, 1);
            }
        }
    }


    close(f1);
    close(f2);

}

void change_lib (const char *file_name_1, const char *file_name_2, const char *n1, const char *n2) {
    //printf("Files: %s %s\n", file_name_1, file_name_2);
    FILE *f1 = fopen(file_name_1, "r");
    FILE *f2 = fopen(file_name_2, "rw+");
    if(f1 == NULL || f2 == NULL) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("LIBRARY");

    char cr;
    while(fread(&cr, sizeof (char), 1, f1) != 0) {
        if(cr != n1[0]) {
            fwrite(&cr, sizeof (char), 1, f2);
        } else {
            size_t id = 1;
            while(id < strlen(n1) && fread(&cr, sizeof (char), 1, f1) != 0) {
                if(cr != n1[id]) break;
                id++;
            }
            if(id == strlen(n1)) {
                fwrite(n2, sizeof (char), strlen(n2), f2);
            } else {
                fwrite(n1, sizeof (char), id, f2);
                fwrite(&cr, sizeof (char), 1, f2);
            }
        }
    }

    fclose(f1);
    fclose(f2);

}

int main(int argc, char **argv) {
    puts("start");

    char *file_name_1 = calloc(256, sizeof (char));
    char *file_name_2 = calloc(256, sizeof (char));

    if(argc == 5) {
        strcpy(file_name_1, argv[1]);
        strcpy(file_name_2, argv[2]);
    } else {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 0;
    }

    struct tms operation_time[5]; //usr and sys
    clock_t operation_time_real[5]; //real

    times(&operation_time[0]);
    operation_time_real[0] = clock();

    //change_lib(file_name_1, file_name_2, argv[3], argv[4]);

    times(&operation_time[1]);
    operation_time_real[1] = clock();

    change_sys(file_name_1, file_name_2, argv[3], argv[4]);

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