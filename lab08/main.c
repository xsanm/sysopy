#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void numbers_method(int rows, int columns,  int **matrix,  int **matrix_res) {

}

void block_method(int rows, int columns,  int **matrix,  int **matrix_res) {

}


int main(int argc, char **argv) {
    if(argc != 5) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    int thread_number = strtol(argv[1], NULL, 10);
    char *mode = argv[2];
    char *input = argv[3];
    char *output = argv[4];



    FILE *f_in = fopen(input, "rb");
    FILE *f_out = fopen(output, "w+");

    if(f_in == NULL || f_out == NULL) {
        printf("Error while opening files\n");
        return 1;
    }

    char v[3];
    fgets(v, sizeof(v), f_in);

    int rows, columns;
    fscanf(f_in, "%d %d", &columns, &rows);

    int gray_val;
    fscanf(f_in, "%d", &gray_val);


    fprintf(f_out, "P2\n");
    fprintf(f_out, "%d %d\n", columns, rows);
    fprintf(f_out, "%d\n", gray_val);

    int **matrix = malloc(sizeof (int *) * rows);
    for(int i = 0; i < rows; i++) {
        matrix[i] = malloc(sizeof (int) * columns);
    }

    int **matrix_res = malloc(sizeof (int *) * rows);
    for(int i = 0; i < rows; i++) {
        matrix_res[i] = malloc(sizeof (int) * columns);
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            fscanf(f_in, "%d", &matrix[i][j]);
        }
    }

    if(strcmp(mode, "numbers") == 0) {
        numbers_method(rows, columns, matrix, matrix_res);
    } else if(strcmp(mode, "block") == 0) {
        block_method(rows, columns, matrix, matrix_res);
    } else {
        puts("Wrong mode");
        return 1;
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            fscanf(f_in, "%d", &matrix_res[i][j]);
            //fprintf(f_out, "%d ", gray_val - tmp);
        }
        fprintf(f_out, "\n");
    }



    return 0;
}