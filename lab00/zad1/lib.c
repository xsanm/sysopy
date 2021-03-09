#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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

        char merged_name[16] = "merged";
        char nr[4];
        
        sprintf(nr, "%d", i);
       
        strcat(merged_name, nr);
        strcat(merged_name, ".txt");
        pairs[i].merged_adress = malloc(strlen(merged_name) * sizeof(char));
        strcpy(pairs[i].merged_adress, merged_name);

        //printf("Merged file: %s\n", merged_name);

        FILE *f = fopen(merged_name, "w+");

        char * line = NULL;
        size_t len = 0;
        int a = 1, b = 1;
        while(a || b) {
            if(a) {
                if(getline(&line, &len, fp_a) != -1) {
                    fprintf(f, "%s", line);
                } else {
                    a = 0;
                }
            }
            if(b) {
                if(getline(&line, &len, fp_b) != -1) {
                    fprintf(f, "%s", line);
                } else {
                    b = 0;
                }
            }
        }
        
        fclose(fp_a);
        fclose(fp_b);
        fclose(f);
    }
}

int add_block(struct block *main_arr, char *temp_file, int i) {
    int lines = 0;
    size_t len = 0;
    FILE *f = fopen(temp_file, "r");
    
    char **rows = calloc(1024, sizeof(char *));
    
    for(int i = 0; i < 1024; i++) {
        rows[i] = NULL;
    }

    while(getline(&rows[lines], &len, f) != -1) {
        lines++;
    }

    
    main_arr[i].rows = rows;
    main_arr[i].number_of_rows = lines;

    fclose(f);

    return i;
}

int rows_in_block(struct block *main_arr, int id) {
    return main_arr[id].number_of_rows;
}

void del_block(struct block *main_arr, int id) {
    free(main_arr[id].rows);
    main_arr[id].rows = NULL;
}

void del_row_from_block(struct block *main_arr, int block_id, int row_id) {
    free(main_arr[block_id].rows[row_id]);
    main_arr[block_id].rows[row_id] = NULL;
}

void display_table(struct block *main_arr, int blocks) {
    for(int i = 0; i < blocks; i++) {
        printf("### BLOCK NR %d:\n", i);
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
