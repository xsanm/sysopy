#ifndef _LIB_H
#define _Lib_H

struct block {
    char **rows;
    int number_of_rows;
};

struct pair {
    char *a_adress;
    char *b_adress;
    char *merged_adress; 
    int rows;
};

struct block *create_blocks(int number_of_blocks);

struct pair *create_pairs(int number_of_pairs);

void merge(struct pair *pairs, int number_of_pairs); //returns name of temp file

int add_block(struct block *main_arr, char *temp_file, int i, int rows_num); //returns id in main arr

int rows_in_block(struct block *main_arr, int id);

void del_block(struct block *main_arr, int id);

void del_row_from_block(struct block *main_arr, int block_id, int row_id);

void display_table(struct block *main_arr, int blocks);

void display_pairs(struct pair *pairs, int number_of_pairs);



#endif //_LIB_H