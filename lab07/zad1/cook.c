#include "shared.h"


int main(int argc, char **argv) {
    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    srand(time(NULL) ^ (getpid()<<16));

    int ID = strtol (argv[1], NULL, 10);

    printf("COOK [%d] hired, PID [%d]\n", ID, getpid());

    struct bake *bake_block = (struct bake *)add_mem_block(BAKE_FNAME, BAKE_ID, BLOCK_SIZE);
    if(bake_block == NULL) {
        puts("ERROR getting block");
    }

    struct table *table_block = (struct table *) add_mem_block(TABLE_FNAME, TABLE_ID, BLOCK_SIZE);
    if(table_block == NULL) {
        puts("ERROR getting block");
    }


    key_t key;
    if ((key = ftok(WORKERS_FNAME, WORKERS_ID)) == -1) {
        perror("ftok");
        exit(1);
    }

    int semaphore_id;
    if ((semaphore_id = semget(key, 2, 0)) == -1) {
        perror("semget");
        exit(1);
    }

    struct sembuf *sops_bake = malloc(sizeof(struct sembuf));
    sops_bake->sem_num = 0;
    sops_bake->sem_op = -1;
    sops_bake->sem_flg = 0;

    struct sembuf *sops_table = malloc(sizeof(struct sembuf));
    sops_table->sem_num = 1;
    sops_table->sem_op = -1;
    sops_table->sem_flg = 0;

    while(1) {
        usleep(rand_time);
        int pizza_type = rand_number(0, 9);
        int shelf_id;
        printf("C [%d] [%d] [%s]\nPrzygotowuje pizze: %d\n", ID, getpid(), timestamp(), pizza_type);

        sleep(2);

        lock(semaphore_id, sops_bake);
        while(bake_block->pizzas >= BAKE_SIZE) {
            unlock(semaphore_id, sops_bake);

            sleep(1);

            lock(semaphore_id, sops_bake);
        }
        shelf_id = bake_block->next_shelf++;
        bake_block->next_shelf %= BAKE_SIZE;
        bake_block->bake_shelves[shelf_id] = pizza_type;
        bake_block->pizzas++;
        printf("C [%d] [%d] [%s]\nDodalem pizze: %d, liczba pizz w piecu: %d\n", ID, getpid(), timestamp(), pizza_type, bake_block->pizzas);
        unlock(semaphore_id, sops_bake);

        sleep(4);

        lock(semaphore_id, sops_bake);
        pizza_type = bake_block->bake_shelves[shelf_id];
        bake_block->bake_shelves[shelf_id] = -1;
        bake_block->pizzas--;
        printf("C [%d] [%d] [%s]\nWyjalem pizze: %d, liczba pizz w piecu: %d\n", ID, getpid(), timestamp(), pizza_type, bake_block->pizzas);
        unlock(semaphore_id, sops_bake);

        lock(semaphore_id, sops_table);
        table_block->pizza_boxes[table_block->next_box++] = pizza_type;
        table_block->next_box %= TABLE_SIZE;
        table_block->pizzas++;
        printf("C [%d] [%d] [%s]\nWYkladam na stol: %d, liczba pizz na stole: %d\n", ID, getpid(), timestamp(), pizza_type, table_block->pizzas);
        unlock(semaphore_id, sops_table);
    }


    //remove_mem_block(block);

    return 0;
}