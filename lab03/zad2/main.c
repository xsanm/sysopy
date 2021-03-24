#define  _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>

#include "lib.h"

double calculate_time_clocks(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC;
}

double calculate_time_tics(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

char *generate_random_line() {
    const int SIZE = (rand() % 16) + 16;
    char *base = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz";
    size_t base_len = strlen(base);

    char *res = (char *) calloc(SIZE + 1, sizeof(char));

    for (int i = 0; i < SIZE - 2; i++) {
        res[i] = base[rand() % base_len];
    }
    
    res[SIZE - 2] = '\n';
    res[SIZE - 1] = '\0';
    return res;
}

void generate_file(char *f_name, int rows_number) {
    FILE *f = fopen(f_name, "w+");
    for(int i = 0; i < rows_number; i++) {
        fprintf(f, "%s", generate_random_line());
    }
    fclose(f);
}

void run_time_test(int blocks, int rows_number) {
    struct tms operation_time[5]; //usr and sys
    clock_t operation_time_real[5]; //real

    //struct block *table = create_blocks(blocks);
    struct pair *pairs = create_pairs(blocks);
    fflush(stdout);
    for(int i = 0; i < blocks; i++) {
        char f_name[32] = "a_test_";
        char nr[16];
        sprintf(nr, "%d", i);
        strcat(f_name, nr);
        strcat(f_name, ".txt");
        
        generate_file(f_name, rows_number);
        pairs[i].a_adress = calloc(strlen(f_name) + 1, sizeof(char));
        strcpy(pairs[i].a_adress, f_name);

        f_name[0] = 'b';
        generate_file(f_name, rows_number);
        pairs[i].b_adress = calloc(strlen(f_name) + 1, sizeof(char));
        strcpy(pairs[i].b_adress, f_name);
    }

    times(&operation_time[0]);
    operation_time_real[0] = clock();

    for(int i = 0; i < blocks; i++) {
        if(fork() == 0) {
            //printf("Child-process nr: %d, pid: %d, ppid %d\n", i, getpid(), getppid());
            merge(pairs, i);
            exit(0);
        }
    }

    for(int i = 0; i < blocks; i++) {
        wait(NULL);
    }


    times(&operation_time[1]);
    operation_time_real[1] = clock();


    puts("              REAl        USER        SYSTEM");
    printf("MERGE       %lf", calculate_time_clocks(operation_time_real[0], operation_time_real[1]));
    printf("    %lf", calculate_time_tics(operation_time[0].tms_utime, operation_time[1].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[0].tms_stime, operation_time[1].tms_stime));

    //printf("SAVE        %lf", calculate_time_clocks(operation_time_real[1], operation_time_real[2]));
    //printf("    %lf", calculate_time_tics(operation_time[1].tms_utime, operation_time[2].tms_utime));
    //printf("    %lf\n", calculate_time_tics(operation_time[1].tms_stime, operation_time[2].tms_stime));

    //printf("DEL         %lf", calculate_time_clocks(operation_time_real[2], operation_time_real[3]));
    //printf("    %lf", calculate_time_tics(operation_time[2].tms_utime, operation_time[3].tms_utime));
    //printf("    %lf\n", calculate_time_tics(operation_time[2].tms_stime, operation_time[3].tms_stime));

    //printf("SAVE-DEL    %lf", calculate_time_clocks(operation_time_real[3], operation_time_real[4]));
    //printf("    %lf", calculate_time_tics(operation_time[3].tms_utime, operation_time[4].tms_utime));
    //printf("    %lf\n", calculate_time_tics(operation_time[3].tms_stime, operation_time[4].tms_stime));


    //free(table);
    free(pairs);
}

int main(int argc, char **argv) {
    srand((unsigned int) time(NULL));

    //printf("%d, %d\n", TMP_MAX, FOPEN_MAX);

    if(argc >= 2 && strcmp(argv[1], "tests") == 0) {
        puts("\n### SMALL TESTS V1 ### ");
        puts("5 FILES, 10 ROWS");
        run_time_test(5, 10);

        puts("\n### SMALL TESTS V2 ### ");
        puts("5 FILES, 100 ROWS");
        run_time_test(5, 100);

        puts("\n### MEDIUM TESTS V1 ### ");
        puts("200 FILES, 100 ROWS");
        run_time_test(200, 100);

        puts("\n### MEDIUM TESTS V2 ### ");
        puts("200 FILES, 400 ROWS");
        run_time_test(200, 400);

        puts("\n### LARGE TESTS V1 ### ");
        puts("1000 FILES, 100 ROWS");
        run_time_test(1000, 100);

        puts("\n### LARGE TESTS V2 ### ");
        puts("1000 FILES, 600 ROWS");
        run_time_test(1000, 600);
    
        return 0;
    }

    struct block *table = NULL;
    int blocks;
    struct pair *pairs = NULL;
    

    if(argc <= 4) {
        puts("NOT ENOUGH ARGUMENTS");
        return 1;
    } else if(strcmp(argv[1], "create_table")) {
        puts("YOU HAVE TO CREATE TABLE");
        return 1;
    } else if(strcmp(argv[3], "merge_files")) {
        puts("YOU HAVE TO MERGE FILES");
        return 1;
    }

    blocks = strtol(argv[2], NULL, 10);

    table = create_blocks(blocks);
    pairs = create_pairs(blocks);
    
    int i = 4;
    for(int j = 0; j < blocks; j++) {
        char *pair = argv[i++];
        int position = (int)(strchr(pair, ':') - pair);
        
        pairs[j].a_adress =  calloc((position + 1), sizeof(char));
        pairs[j].b_adress =  calloc((strlen(pair) - position + 1), sizeof(char));
        sscanf(pair, "%[^:]:%s", pairs[j].a_adress, pairs[j].b_adress);

    }

    for(int i = 0; i < blocks; i++) {
        if(fork() == 0) {
            printf("Child-process nr: %d, pid: %d, ppid %d\n", i, getpid(), getppid());
            merge(pairs, i);
            return 0;
        }
    }

    for(int i = 0; i < blocks; i++) {
        wait(NULL);
    }

    free(table);
    free(pairs);
    return 0;
}