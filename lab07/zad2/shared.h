#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

sem_t *semaphores[2];

#define BAKE_SEM "/bake_sem"
#define TABLE_SEM "/table_sem"

#define min_sleep 100
#define max_sleep 1000

#define BAKE_FNAME "/bake"
#define TABLE_FNAME "/table"

#define BAKE_SIZE 5
#define TABLE_SIZE 5

#define BLOCK_SIZE 4096

#define rand_number(_min, _max) (rand() %(_max - _min + 1)) + _min

#define rand_time ((rand() % (max_sleep - min_sleep + 1) + min_sleep) * 1000)

union semun {
    int val;
    struct semid_ds *buf;
    short *array;
};

struct bake {
    int bake_shelves[BAKE_SIZE]; //-1 empty, 0-9 pizza nr
    int pizzas;
    int next_shelf;
};

struct table {
    int pizza_boxes[TABLE_SIZE]; //-1 empty, 0-9 pizza nr
    int pizzas;
    int next_box;
    int box_to_pick;
};

char * timestamp();

#endif