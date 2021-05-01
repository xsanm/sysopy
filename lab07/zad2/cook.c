#include "shared.h"


int main(int argc, char **argv) {
    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    srand(time(NULL) ^ (getpid()<<16));

    int ID = strtol (argv[1], NULL, 10);

    printf("COOK [%d] hired, PID [%d]\n", ID, getpid());

    int fd_bake, fd_table;
    if ((fd_bake = shm_open(BAKE_FNAME, O_RDWR, 0760)) == - 1) {
        perror("shm_open");
        exit(1);
    }
    if ((fd_table = shm_open(TABLE_FNAME, O_RDWR, 0760)) == - 1) {
        perror("shm_open");
        exit(1);
    }

    sem_t *sem_bake, *sem_table;
    if ((sem_bake = sem_open(BAKE_SEM, O_RDWR)) == (void*)- 1){
        perror("shm_open");
        exit(1);
    }

    if ((sem_table = sem_open(TABLE_SEM, O_RDWR)) == (void*)- 1){
        perror("shm_open");
        exit(1);
    }

    while(1) {
        usleep(rand_time);
        int pizza_type = rand_number(0, 9);
        int shelf_id;
        printf("C [%d] [%d] [%s]\nPrzygotowuje pizze: %d\n", ID, getpid(), timestamp(), pizza_type);

        sleep(2);

        sem_wait(sem_bake);
        struct bake *bake_block = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bake, 0);
        while(bake_block->pizzas >= BAKE_SIZE) {
            munmap(bake_block, BLOCK_SIZE);
            sem_post(sem_bake);

            sleep(1);

            sem_wait(sem_bake);
            bake_block = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bake, 0);
        }
        shelf_id = bake_block->next_shelf++;
        bake_block->next_shelf %= BAKE_SIZE;
        bake_block->bake_shelves[shelf_id] = pizza_type;
        bake_block->pizzas++;
        printf("C [%d] [%d] [%s]\nDodalem pizze: %d, liczba pizz w piecu: %d\n", ID, getpid(), timestamp(), pizza_type, bake_block->pizzas);
        munmap(bake_block, BLOCK_SIZE);
        sem_post(sem_bake);

        sleep(4);

        sem_wait(sem_bake);
        bake_block = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bake, 0);
        pizza_type = bake_block->bake_shelves[shelf_id];
        bake_block->bake_shelves[shelf_id] = -1;
        bake_block->pizzas--;
        printf("C [%d] [%d] [%s]\nWyjalem pizze: %d, liczba pizz w piecu: %d\n", ID, getpid(), timestamp(), pizza_type, bake_block->pizzas);
        munmap(bake_block, BLOCK_SIZE);
        sem_post(sem_bake);

        sem_wait(sem_table);
        struct table *table_block=  mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_table, 0);
        table_block->pizza_boxes[table_block->next_box++] = pizza_type;
        table_block->next_box %= TABLE_SIZE;
        table_block->pizzas++;
        printf("C [%d] [%d] [%s]\nWYkladam na stol: %d, liczba pizz na stole: %d\n", ID, getpid(), timestamp(), pizza_type, table_block->pizzas);
        munmap(table_block, BLOCK_SIZE);
        sem_post(sem_table);
    }


    //remove_mem_block(block);

    return 0;
}