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

#define min_sleep 100
#define max_sleep 1000

#define BAKE_FNAME "bake"
#define TABLE_FNAME "table"

#define BAKE_ID 'B'
#define TABLE_ID 'T'

#define WORKERS_FNAME "main.c"
#define WORKERS_ID 'W'

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

void lock(int semaphore_id, struct sembuf *sb);
void unlock(int semaphore_id, struct sembuf *sb);

char * timestamp();

int get_mem_block(char *fname, int proj_id, int size);
char *add_mem_block(char *fname, int proj_id, int size);
void remove_mem_block(char *block);

#endif