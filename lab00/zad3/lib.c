#define  _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "lib.h"


struct block *create_blocks(int number_of_blocks) {
    if (number_of_blocks < 0) {
        return NULL;
    }
    struct block *res = calloc(number_of_blocks, sizeof(struct block));
    return res;
}

struct pair *create_pairs(int number_of_pairs) {
    if (number_of_pairs < 0) {
        return NULL;
    }
    struct pair *res = calloc(number_of_pairs, sizeof(struct pair));
    return res;
}

void merge(struct pair *pairs, int number_of_pairs) {
    for(int i = 0; i < number_of_pairs; i++) {
        FILE * fp_a = fopen(pairs[i].a_adress, "r");
        FILE * fp_b = fopen(pairs[i].b_adress, "r");
        if (fp_a == NULL || fp_b == NULL) {
            printf("ERROR WHILE READING pair: %s:%s\n", pairs[i].a_adress, pairs[i].b_adress);
            return;
        }

        pairs[i].tmp = tmpfile();

        char * line = NULL;
        size_t len = 0;
        int a = 1, b = 1;
        int rows = 0;
        while(a || b) {
            if(pairs[i].tmp == NULL) break;
            if(a) {
                if(getline(&line, &len, fp_a) != -1) {
                    fprintf(pairs[i].tmp, "%s", line);
                    free(line);
                    line = NULL;
                    len = 0;
                    rows++;
                } else {
                    a = 0;
                }
            }
            if(b) {
                if(getline(&line, &len, fp_b) != -1) {
                    fprintf(pairs[i].tmp, "%s", line);
                    free(line);
                    line = NULL;
                    len = 0;
                    rows++;
                } else {
                    b = 0;
                }
            }
        }
        pairs[i].rows = rows;
    
        fclose(fp_a);
        fclose(fp_b);
    }
}

int add_block(struct block *main_arr, FILE* tmp, int id, int rows_num) {
    int lines = 0;
    size_t len = 0;
    char **rows = calloc(rows_num, sizeof(char *));
    for(int i = 0; i < rows_num; i++) rows[i] = NULL;

    rewind(tmp);

    while(getline(&rows[lines], &len, tmp) != -1) {
        lines++;
    }

    main_arr[id].rows = rows;
    main_arr[id].number_of_rows = lines;

    return id;
}

int rows_in_block(struct block *main_arr, int id) {
    return main_arr[id].number_of_rows;
}

void del_block(struct block *main_arr, int id) {
    if(main_arr[id].rows == NULL) return;
    for(int i = 0; i < main_arr[id].number_of_rows; i++) {
        free(main_arr[id].rows[i]);
        main_arr[id].rows[i] = NULL;
    }
    free(main_arr[id].rows);
    main_arr[id].rows = NULL;
}

void del_row_from_block(struct block *main_arr, int block_id, int row_id) {
    if(main_arr[block_id].rows == NULL || main_arr[block_id].number_of_rows <= row_id) return;
    free(main_arr[block_id].rows[row_id]);
    main_arr[block_id].rows[row_id] = NULL;
}

void display_table(struct block *main_arr, int blocks) {
    for(int i = 0; i < blocks; i++) {
        printf("\nBLOCK NR %d:\n", i);
        if(main_arr[i].rows == NULL) {
            puts("DELETED");
            continue;
        }
        for(int j = 0; j < main_arr[i].number_of_rows; j++) {
            if(main_arr[i].rows[j] != NULL) printf("%s", main_arr[i].rows[j]);
            else puts("DELETED");
        }
    }
}

void display_pairs(struct pair *pairs, int number_of_pairs) {
    for(int i = 0; i < number_of_pairs; i++) {
        printf("Pair %d: %s <> %s\n", i, pairs[i].a_adress, pairs[i].b_adress);
    }
}
