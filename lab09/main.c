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


int main() {
    puts("START");
    srand(time(NULL));



    return 0;
}