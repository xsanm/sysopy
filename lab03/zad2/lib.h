#ifndef _LIB_H
#define _Lib_H

struct block {
    char **rows;
    int number_of_rows;
};

struct pair {
    char *a_adress;
    char *b_adress;
    FILE* tmp; 
    int rows;
};

struct block *create_blocks(int number_of_blocks); //Utworzenie tablicy

struct pair *create_pairs(int number_of_pairs); //Definiowanie sekwencji par 

void merge(struct pair *pairs, int id); //Przeprowadzenie zmergowania

int add_block(struct block *main_arr, FILE* tmp, int i, int rows_num); //utowrzenie bloku

int rows_in_block(struct block *main_arr, int id); //informacjia o ilosci wierszy 

void del_block(struct block *main_arr, int id); //usuniecie bloku

void del_row_from_block(struct block *main_arr, int block_id, int row_id); //usuniecie wiersza

void display_table(struct block *main_arr, int blocks); //wypisanie blokow

void display_pairs(struct pair *pairs, int number_of_pairs); //wypisanie par



#endif 