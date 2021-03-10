#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>

#define  _GNU_SOURCE

#include "lib.h"
void *dl_handle;

struct block *(*dl_create_blocks)();
struct pair *(*dl_create_pairs)();
void (*dl_merge)();
int (*dl_add_block)();
int (*dl_rows_in_block)();
void (*dl_del_block)();
void (*dl_del_row_from_block)();
void (*dl_display_table)();
void (*dl_display_pairs)();

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

    char *res = (char *) malloc(SIZE * sizeof(char));

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
    struct tms operation_time[8]; //usr and sys
    clock_t operation_time_real[8]; //real

    struct block *table = dl_create_blocks(blocks);
    struct pair *pairs = dl_create_pairs(blocks);

    for(int i = 0; i < blocks; i++) {
        char f_name[32] = "a_test_";
        char nr[16];
        sprintf(nr, "%d", i);
        strcat(f_name, nr);
        strcat(f_name, ".txt");
        
        generate_file(f_name, rows_number);
        pairs[i].a_adress = malloc(strlen(f_name) * sizeof(char));
        strcpy(pairs[i].a_adress, f_name);

        f_name[0] = 'b';
        generate_file(f_name, rows_number);
        pairs[i].b_adress = malloc(strlen(f_name) * sizeof(char));
        strcpy(pairs[i].b_adress, f_name);
    }

    times(&operation_time[0]);
    operation_time_real[0] = clock();

    dl_merge(pairs, blocks);

    times(&operation_time[1]);
    operation_time_real[1] = clock();

    for(int j = 0; j < blocks; j++) {
        dl_add_block(table, pairs[j].merged_adress, j, pairs[j].rows);
    }

    times(&operation_time[2]);
    operation_time_real[2] = clock();

    for(int j = 0; j < blocks; j++) {
        dl_del_block(table, j);
    }

    times(&operation_time[3]);
    operation_time_real[3] = clock();

    for(int k = 0; k <= 10; k++) {
        for(int j = 0; j < blocks; j++) {
            dl_add_block(table, pairs[j].merged_adress, j, pairs[j].rows);
        }
        for(int j = 0; j < blocks; j++) {
            dl_del_block(table, j);
        }   
    }
    times(&operation_time[4]);
    operation_time_real[4] = clock();

    puts("              REAl        USER        SYSTEM");
    printf("MERGE       %lf", calculate_time_clocks(operation_time_real[0], operation_time_real[1]));
    printf("    %lf", calculate_time_tics(operation_time[0].tms_utime, operation_time[1].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[0].tms_stime, operation_time[1].tms_stime));

    printf("SAVE        %lf", calculate_time_clocks(operation_time_real[1], operation_time_real[2]));
    printf("    %lf", calculate_time_tics(operation_time[1].tms_utime, operation_time[2].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[1].tms_stime, operation_time[2].tms_stime));

    printf("DEL         %lf", calculate_time_clocks(operation_time_real[2], operation_time_real[3]));
    printf("    %lf", calculate_time_tics(operation_time[2].tms_utime, operation_time[3].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[2].tms_stime, operation_time[3].tms_stime));

    printf("SAVE-DEL    %lf", calculate_time_clocks(operation_time_real[3], operation_time_real[4]));
    printf("    %lf", calculate_time_tics(operation_time[3].tms_utime, operation_time[4].tms_utime));
    printf("    %lf\n", calculate_time_tics(operation_time[3].tms_stime, operation_time[4].tms_stime));

}

int main(int argc, char **argv) {
    
    dl_handle = dlopen("./lib_shared.so", RTLD_LAZY);
    if(dl_handle == NULL){
        printf("Cannot find library");
        return -1;
    }

    dl_create_blocks = dlsym(dl_handle, "create_blocks");
    dl_create_pairs = dlsym(dl_handle, "create_pairs");
    dl_merge = dlsym(dl_handle, "merge");
    dl_add_block = dlsym(dl_handle, "add_block");
    dl_rows_in_block = dlsym(dl_handle, "rows_in_block");
    dl_del_block = dlsym(dl_handle, "del_block");
    dl_del_row_from_block = dlsym(dl_handle, "del_row_from_block");
    dl_display_table = dlsym(dl_handle, "display_table");
    dl_display_pairs = dlsym(dl_handle, "display_pairs");
    


    srand((unsigned int) time(NULL));

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

    //struct tms operation_time[8];
    //clock_t operation_time_real[8];
    //times(&operation_time[0]);
    //operation_time_real[0] = clock();

    struct block *table = NULL;
    struct pair *pairs = NULL;
    int blocks;

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

    table = dl_create_blocks(blocks);
    pairs = dl_create_pairs(blocks);
    
    int i = 4;
    for(int j = 0; j < blocks; j++) {
        char *pair = argv[i++];
        int position = (int)(strchr(pair, ':') - pair);
        
        pairs[j].a_adress =  calloc((position + 1), sizeof(char));
        pairs[j].b_adress =  calloc((strlen(pair) - position + 1), sizeof(char));
        sscanf(pair, "%[^:]:%s", pairs[j].a_adress, pairs[j].b_adress);

        //strncpy(pairs[j].a_adress, pair, position);
        //strncpy(pairs[j].b_adress, pair + position + 1, (strlen(pair) - position));
    }

    //display_pairs(pairs, blocks);

    dl_merge(pairs, blocks);

    for(int j = 0; j < blocks; j++) {
        dl_add_block(table, pairs[j].merged_adress, j, pairs[j].rows);
    }

    puts("\n### INITAL BLOCKS ###");
    dl_display_table(table, blocks);

    while(i < argc) {
        if(strcmp(argv[i], "remove_block") == 0) {
            int block_id = strtol(argv[i + 1], NULL, 10);
            if(block_id < 0 || block_id >= blocks) {
                puts("WRONG BLOCK ID");
            } else {
                dl_del_block(table, block_id);
            }
           i++;
        } else if(strcmp(argv[i], "remove_row") == 0) {
            int block_id = strtol(argv[i + 1], NULL, 10);
            int row_id = strtol(argv[i + 2], NULL, 10);
            if(block_id < 0 || block_id >= blocks) {
                puts("WRONG BLOCK ID");
            } else if(row_id < 0 || row_id >= dl_rows_in_block(table, block_id)){
                puts("WRONG ROW ID");
            } else {
                dl_del_row_from_block(table, block_id, row_id);
            }
            i += 2;
        } else {
            puts("invalid instruction");
        }
        i++;
    }

    puts("\n### FINAL BLOCKS ###");
    dl_display_table(table, blocks);
    puts("");

    return 0;
}