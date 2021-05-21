#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NO_REINDEERS 9
#define NO_ELVES 10
#define NO_ELVES_WAITING 3

#define T_ELF_WORKING rand() % 4 + 2
#define T_SANTA_SOLVING rand() % 2 + 1
#define T_REINDEER_HOLIDAY rand() % 6 + 2
#define T_REINDEER_DELIVERING rand() % 3 + 1

void *santa(void *arg) {

    return NULL;
}

void* reindeer(void* arg) {
    int id = *((int *) arg);
    printf("Reindeer no [%d] alive\n", id);

    return NULL;
}

void* elf(void* arg) {
    int id = *((int *) arg);
    printf("Elf no [%d] alive\n", id);

    return NULL;
}

int main() {
    puts("START");
    srand(time(NULL));

    //starting machine
    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, &santa, NULL);

    pthread_t reindeer_thread[NO_REINDEERS];
    for(int i = 0; i < NO_REINDEERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&reindeer_thread[i], NULL, &reindeer, id);
    }

    pthread_t elf_thread[NO_ELVES];
    for(int i = 0; i < NO_ELVES; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&elf_thread[i], NULL, &elf, id);
    }

    //wait for end
    pthread_join(santa_thread, NULL);
    for(int i = 0; i < NO_REINDEERS; i++) {
        pthread_join(reindeer_thread[i], NULL);
    }
    for(int i = 0; i < NO_ELVES; i++) {
        pthread_join(elf_thread[i], NULL);
    }

    return 0;
}