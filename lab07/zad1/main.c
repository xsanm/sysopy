#include "shared.h"

void create_semaphore() {
    key_t key;
    if ((key = ftok(WORKERS_FNAME, WORKERS_ID)) == -1) {
        perror("ftok");
        exit(1);
    }

    int semid;
    if ((semid = semget(key, 2, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
        exit(1);
    }

    union semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1 || semctl(semid, 1, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }
}

int create_mem_block() {
    key_t key_b = ftok(BAKE_FNAME, BAKE_ID);
    if(key_b == -1) {
        puts("ftok error");
        exit(1);
    }
    int id_b = shmget(key_b, BLOCK_SIZE, 0644 | IPC_CREAT);

    struct bake *bake_block = shmat(id_b, NULL, 0);
    bake_block->pizzas = 0;
    bake_block->next_shelf = 0;
    for(int i = 0; i < BAKE_SIZE; i++) {
        bake_block->bake_shelves[i] = -1;
    }

    key_t key_t = ftok(TABLE_FNAME, TABLE_ID);
    if(key_b == -1) {
        puts("ftok error");
        exit(1);
    }
    int id_t = shmget(key_t, BLOCK_SIZE, 0644 | IPC_CREAT);

    struct table *table_block = shmat(id_t, NULL, 0);
    table_block->pizzas = 0;
    table_block->next_box = 0;
    table_block->box_to_pick = 0;
    for(int i = 0; i < TABLE_SIZE; i++) {
        table_block->pizza_boxes[i] = -1;
    }


    return 0;
}

int get_mem_block(char *fname, int proj_id, int size) {
    key_t key = ftok(fname, proj_id);
    if(key == -1) {
        puts("ftok error");
        exit(1);
    }
    return shmget(key, size, 0644);
}

void destroy_mem_block(char *fname) {
    int block_id = get_mem_block(fname, WORKERS_ID, BLOCK_SIZE);
    if(block_id == -1) {
        puts("destoying problem");
        return;
    }
    if(shmctl(block_id, IPC_RMID, NULL) == -1) {
        puts("shmctl error");
    }
}


void run_pizzeria(int cooks_number, int supplier_number) {
    for (int i = 0; i < cooks_number; i++) {
        pid_t child_pid = fork();
        char snum[5];
        sprintf(snum, "%d", i + 1);
        if (child_pid == 0) {
            execlp("./cook",  "./cook", snum, NULL);
        }
    }

    for (int i = 0; i < supplier_number; i++) {
        pid_t child_pid = fork();
        char snum[5];
        sprintf(snum, "%d", i + 1);
        if (child_pid == 0) {
            execlp("./supplier",  "./supplier", snum, NULL);
        }
    }
    for(int i = 0; i < cooks_number + supplier_number; i++) {
        wait(NULL);
    }
}


int main(int argc, char **argv) {
    if(argc != 3) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    int N = strtol (argv[1], NULL, 10);
    int M = strtol (argv[2], NULL, 10);
    printf("STARTING PIZZERIA with %d cooks and %d suppliers\n", N, M);

    create_semaphore();
    create_mem_block();

    run_pizzeria(N, M);

    //destroy_mem_block(FNAME);

    return 0;
}


