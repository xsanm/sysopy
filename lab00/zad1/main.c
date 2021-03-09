#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"



int main(int argc, char **argv) {

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

    table = create_blocks(blocks);
    pairs = create_pairs(blocks);
    
    int i = 4;
    for(int j = 0; j < blocks; j++) {
        char *pair = argv[i++];
        int position = (int)(strchr(pair, ':') - pair);

        pairs[j].a_adress = (char*) malloc((position + 1) * sizeof(char));
        pairs[j].b_adress = (char*) malloc((strlen(pair) - position + 1) * sizeof(char));

        strncpy(pairs[j].a_adress, pair, position);
        strncpy(pairs[j].b_adress, pair + position + 1, (strlen(pair) - position));
    }

    display_pairs(pairs, blocks);

    merge(pairs, blocks);

    for(int j = 0; j < blocks; j++) {
        add_block(table, pairs[j].merged_adress, j);
    }

    display_table(table, blocks);


    //TODO sprawdzic czy wgl mozna usunac

    while(i < argc) {
        if(strcmp(argv[i], "remove_block") == 0) {
            int block_id = strtol(argv[i + 1], NULL, 10);
            if(block_id < 0 || block_id >= blocks) {
                puts("WRONG BLOCK ID");
            } else {
                del_block(table, block_id);
            }
           i++;
        } else if(strcmp(argv[i], "remove_row") == 0) {
            int block_id = strtol(argv[i + 1], NULL, 10);
            int row_id = strtol(argv[i + 2], NULL, 10);
            if(block_id < 0 || block_id >= blocks) {
                puts("WRONG BLOCK ID");
            } else if(row_id < 0 || row_id >= rows_in_block(table, block_id)){
                puts("WRONG ROW ID");
            } else {
                del_row_from_block(table, block_id, row_id);
            }
            i += 2;
        } else {
            puts("invalid instruction");
        }
        i++;
    }

    display_table(table, blocks);

    return 0;
}