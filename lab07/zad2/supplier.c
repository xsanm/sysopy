#include "shared.h"

void sigint_handler(int signum) {
    sem_unlink(TABLE_SEM);
    shm_unlink(TABLE_FNAME);
    exit(0);
}

int main(int argc, char **argv) {
    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    signal(SIGINT, sigint_handler);

    int ID = strtol (argv[1], NULL, 10);
    printf("Supplier [%d] hired, PID[%d]\n", ID, getpid());

    int fd_table;
    if ((fd_table = shm_open(TABLE_FNAME, O_RDWR | O_CREAT, 0760)) == - 1) {
        perror("shm_open");
        exit(1);
    };

    sem_t *sem_table;
    if ((sem_table = sem_open(TABLE_SEM, O_RDWR)) == (void*)- 1){
        perror("shm_open");
        exit(1);
    }

    while(1) {
        usleep(rand_time);
        sem_wait(sem_table);
        struct table *table_block=  mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_table, 0);

        if(table_block->pizzas > 0) {
            int pizza_id = table_block->pizza_boxes[table_block->box_to_pick];
            printf("S [%d] [%d] [%s]\nBiore pizze: %d, liczba pizz na stole: %d\n", ID, getpid(), timestamp(), pizza_id, table_block->pizzas--);
            table_block->pizza_boxes[table_block->box_to_pick++] = -1;
            table_block->box_to_pick %= TABLE_SIZE;
            munmap(table_block, BLOCK_SIZE);
            sem_post(sem_table);

            sleep(5);
            printf("S [%d] [%d] [%s]\n Dostarczam %d\n", ID, getpid(), timestamp(), pizza_id);

            sleep(5);
        } else {
            munmap(table_block, BLOCK_SIZE);
            sem_post(sem_table);
        }
    }

    return 0;
}




