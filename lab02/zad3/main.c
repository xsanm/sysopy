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

void cp_sys(const char *file_name) {
    printf("File and char: %s \n", file_name);
    int f = open(file_name, O_RDONLY);
    if(f < 0) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("SYSTEM");

    long long number;
    size_t readed = read(f, &number, 1);

    while(readed != 0) {
        printf("%lld\n", number);
        readed = read(f, &number, 1);
    }

    close(f);
}

void cp_lib(const char *file_name) {
    printf("File and char: %s \n", file_name);
    FILE *f = fopen(file_name, "rb");
    if(f == NULL) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    puts("LIBRARY");

    size_t readed = fread(&number, sizeof (int), 1, f);
    printf("%d\n", readed);

    while(readed != 0) {
        printf("%d\n", number);
        readed = fread(&number, sizeof (int), 1, f);
    }

    fclose(f);
}

int main(int argc, char **argv) {

    char *file_name = calloc(256, sizeof (char));

    if(argc == 2) {
        strcpy(file_name, argv[1]);
    } else {
        strcpy(file_name, "dane.txt");
    }

    struct tms operation_time[5]; //usr and sys
    clock_t operation_time_real[5]; //real

    times(&operation_time[0]);
    operation_time_real[0] = clock();

    cp_lib(file_name);
    puts("");

    times(&operation_time[1]);
    operation_time_real[1] = clock();

    cp_sys(file_name);

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