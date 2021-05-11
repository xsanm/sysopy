#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>

int **matrix;
int **matrix_res;
int rows, columns;
int threads;

void czas(struct rusage *ru0, struct rusage *ru1){

    double utime = 0, stime = 0, ttime = 0;
    /* Obliczenie czasow. Aby mikrosekundy traktowac jako czesci sekund musimy je wymnozyc przez 10^-6*/
    utime = (double) ru1->ru_utime.tv_sec
            + 1.e-6 * (double) ru1->ru_utime.tv_usec
            - ru0->ru_utime.tv_sec
            - 1.e-6 * (double) ru0->ru_utime.tv_usec;
    stime = (double) ru1->ru_stime.tv_sec
            + 1.e-6 * (double) ru1->ru_stime.tv_usec
            - ru0->ru_stime.tv_sec
            - 1.e-6 * (double) ru0->ru_stime.tv_usec;
    ttime = stime + utime;

    printf("user time: %3f\n", utime);
    printf("system time: %3f\n", stime);
    printf("total time: %3f\n", ttime);
}

void* number_routine(void *arg) {
    int index = *(int*)arg;

    int cnt = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j <= columns; j++) {
            if((cnt++) % threads == index) {
                matrix_res[i][j] = 255 - matrix[i][j];
            }
        }
    }
    free(arg);
    return NULL;
}

void* block_routine(void *arg) {
    int k = *(int*)arg;

    int a = (k - 1) * ((columns) / threads);
    int b = k * ((columns) / threads) - 1;

    //printf("%d %d\n", a, b);
    for(int i = 0; i < rows; i++) {
        for(int j = a; j <= b; j++) {
            matrix_res[i][j] = 255 - matrix[i][j];
        }
    }
    free(arg);
    return NULL;
}


void run_method(int mode) {
    struct rusage t0, t1, t0_self, t1_self;

    getrusage(RUSAGE_SELF, &t0);
    getrusage(RUSAGE_THREAD, &t0_self);
    pthread_t *th = malloc(sizeof (pthread_t) * threads);

    for(int i = 0; i < threads; i++) {
        int* a = malloc(sizeof(int));
        if(mode == 1) {
            *a = i;
            if (pthread_create(&th[i], NULL, &number_routine, a) != 0) {
                perror("Failed to created thread");
            }
        } else if(mode == 2) {
            *a = i + 1;
            if (pthread_create(&th[i], NULL, &block_routine, a) != 0) {
                perror("Failed to created thread");
            }
        }
    }
    for (int i = 0; i < threads; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    getrusage(RUSAGE_SELF, &t1);
    getrusage(RUSAGE_THREAD, &t1_self);

    if(mode == 1) {
        printf("NUMBER METHOD, THREADS [%d]\n", threads);
    } else {
        printf("BLOCK METHOD, THREADS [%d]\n", threads);
    }

    puts("\nWszystkie zasoby");
    czas(&t0, &t1);
    puts("\nWatek glowny");
    czas(&t0_self, &t1_self);
    puts("");
}

int main(int argc, char **argv) {

    if(argc != 5) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    threads = strtol(argv[1], NULL, 10);
    char *mode = argv[2];
    char *input = argv[3];
    char *output = argv[4];


    FILE *f_in = fopen(input, "rb");
    FILE *f_out = fopen(output, "w+");

    if(f_in == NULL || f_out == NULL) {
        printf("Error while opening files\n");
        return 1;
    }

    //read data
    char v[3];
    fgets(v, sizeof(v), f_in);
    fscanf(f_in, "%d %d", &columns, &rows);

    int gray_val;
    fscanf(f_in, "%d", &gray_val);

    matrix = malloc(sizeof (int *) * rows);
    for(int i = 0; i < rows; i++) {
        matrix[i] = malloc(sizeof (int) * columns);
    }

    matrix_res = malloc(sizeof (int *) * rows);
    for(int i = 0; i < rows; i++) {
        matrix_res[i] = malloc(sizeof (int) * columns);
    }

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            fscanf(f_in, "%d", &matrix[i][j]);
        }
    }

    // do sth

    if(strcmp(mode, "numbers") == 0) {
        run_method(1);
    } else if(strcmp(mode, "block") == 0) {
        run_method(2);
    } else {
        puts("Wrong mode");
        return 1;
    }

    //save results

    fprintf(f_out, "P2\n");
    fprintf(f_out, "%d %d\n", columns, rows);
    fprintf(f_out, "%d\n", gray_val);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            //fscanf(f_in, "%d", &matrix_res[i][j]);
            fprintf(f_out, "%d ", matrix_res[i][j]);
        }
        fprintf(f_out, "\n");
    }

    fclose(f_in);
    fclose(f_out);

    return 0;
}