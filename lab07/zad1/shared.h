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

#define BAKE_FNAME "bake"
#define TABLE_FNAME "table"

#define BAKE_ID 'B'
#define TABLE_ID 'T'

#define WORKERS_FNAME "main.c"
#define WORKERS_ID 'W'

#define BAKE_SIZE 5
#define TABLE_SIZE 5

#define BLOCK_SIZE 4096

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