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

bool is_squared(long long num) {
    long long i = 0;
    while(i * i < num) i++;
    if(i * i == num) return true;
    return false;
}

void cp_sys(const char *file_name) {
    printf("File and char: %s \n", file_name);
    int f = open(file_name, O_RDONLY);
    if(f < 0) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    int a = open("a_sys.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    int b = open("b_sys.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    int c = open("c_sys.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);

    if(a < 0 || b < 0 || c < 0) {
        printf("Error while opening: %s\n", strerror(errno));
        return;
    }
    char endline = '\n';

    puts("SYSTEM");
    //rintf("%d\n", a);
    long long number;
    char cr;
    //int readed;
    char tmp[256] = "                                                \n";
    //char *tmp2 = "dsgsfdghshf";
    //rintf("%d\n", write(a, tmp2, strlen(tmp2)));
    //printf("Error while opening: %s\n", strerror(errno));
    write(a, tmp, strlen(tmp));
    write(b, tmp, strlen(tmp));
    write(c, tmp, strlen(tmp));

    int even = 0, tenth = 0, squares = 0;

    int was_nr;
    size_t readed = 1;

    while(readed != 0) {
        number = 0;
        readed = 0;
        was_nr = 0;
        readed = read(f, &cr, 1);
        while(readed != 0 && cr <= '9' && cr >= '0'){
            number *= 10;
            number += (cr - '0');
            was_nr = 1;
            readed = read(f, &cr, 1);
        }
        if(was_nr == 1) {
            sprintf(tmp, "%lld", number);
            long long tmp_n = (number % 100) / 10;
            if((number & 1) == 0) {
                even++;
                write(a, tmp, strlen(tmp));
                write(a, &endline, 1);
            }
            if(number > 9 && (tmp_n == 0 || tmp_n == 7)) {
                tenth++;
                write(b, tmp, strlen(tmp));
                write(b, &endline, 1);
            }
            if(is_squared(number)) {
                squares++;
                write(c, tmp, strlen(tmp));
                write(c, &endline, 1);
            }
        }
    }

    lseek(a, 0, SEEK_SET);
    lseek(b, 0, SEEK_SET);
    lseek(c, 0, SEEK_SET);

    sprintf(tmp, "%s", "Liczb parzystych jest ");
    write(a, tmp, strlen(tmp));
    sprintf(tmp, "%d", even);
    write(a, tmp, strlen(tmp));

    sprintf(tmp, "%s", "Liczb z 0 lub 7 jako dziesiatki jest ");
    write(b, tmp, strlen(tmp));
    sprintf(tmp, "%d", tenth);
    write(b, tmp, strlen(tmp));

    sprintf(tmp, "%s", "Liczb bedacych kwadratami jest ");
    write(c, tmp, strlen(tmp));
    sprintf(tmp, "%d", squares);
    write(c, &tmp, strlen(tmp));

    close(a);
    close(b);
    close(c);
    close(f);
}

void cp_lib(const char *file_name) {
    printf("File and char: %s \n", file_name);
    FILE *f = fopen(file_name, "rb");
    if(f == NULL) {
        printf("Error while reading: %s\n", strerror(errno));
        return;
    }
    FILE *a = fopen("a_lib.txt", "w+");
    FILE *b = fopen("b_lib.txt", "w+");
    FILE *c = fopen("c_lib.txt", "w+");

    if(a == NULL || b == NULL || c == NULL) {
        printf("Error while opening: %s\n", strerror(errno));
        return;
    }
    char endline = '\n';

    puts("LIBRARY");

    long long number;
    char cr;
    int readed;
    char tmp[256] = "                                               \n";
    fwrite(tmp, sizeof (char), strlen(tmp), a);
    fwrite(tmp, sizeof (char), strlen(tmp), b);
    fwrite(tmp, sizeof (char), strlen(tmp), c);

    int even = 0, tenth = 0, squares = 0;

    while(!feof(f)) {
        number = 0;
        readed = 0;
        while(fread(&cr, sizeof (char), 1, f) && cr <= '9' && cr >= '0'){
            number *= 10;
            number += (cr - '0');
            readed = 1;
        }
        if(readed == 1) {

            sprintf(tmp, "%lld", number);
            long long tmp_n = (number % 100) / 10;
            if((number & 1) == 0) {
                even++;
                fwrite(tmp, sizeof (char), strlen(tmp), a);
                fwrite(&endline, sizeof (char), 1, a);
            }
            if(number > 9 && (tmp_n == 0 || tmp_n == 7)) {
                tenth++;
                fwrite(tmp, sizeof (char), strlen(tmp), b);
                fwrite(&endline, sizeof (char), 1, b);
            }
            if(is_squared(number)) {
                squares++;
                fwrite(tmp, sizeof (char), strlen(tmp), c);
                fwrite(&endline, sizeof (char), 1, c);
            }
        }
    }

    rewind(a);
    rewind(b);
    rewind(c);

    sprintf(tmp, "%s", "Liczb parzystych jest ");
    fwrite(tmp, sizeof (char), strlen(tmp), a);
    sprintf(tmp, "%d", even);
    fwrite(tmp, sizeof (char), strlen(tmp), a);

    sprintf(tmp, "%s", "Liczb z 0 lub 7 jako dziesiatki jest ");
    fwrite(tmp, sizeof (char), strlen(tmp), b);
    sprintf(tmp, "%d", tenth);
    fwrite(tmp, sizeof (char), strlen(tmp), b);

    sprintf(tmp, "%s", "Liczb bedacych kwadratami jest ");
    fwrite(tmp, sizeof (char), strlen(tmp), c);
    sprintf(tmp, "%d", squares);
    fwrite(tmp, sizeof (char), strlen(tmp), c);

    fclose(a);
    fclose(b);
    fclose(c);
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