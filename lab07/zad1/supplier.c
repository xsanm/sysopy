#include "shared.h"

int main(int argc, char **argv) {
    if(argc != 2) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }

    int ID = strtol (argv[1], NULL, 10);
    printf("Supplier [%d] hired, PID[%d]\n", ID, getpid());

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

    struct sembuf *sops_table = malloc(sizeof(struct sembuf));
    sops_table->sem_num = 1;
    sops_table->sem_op = -1;
    sops_table->sem_flg = 0;

    while(1) {
        usleep(rand_time);
        lock(semaphore_id, sops_table);
        if(table_block->pizzas > 0) {
            int pizza_id = table_block->pizza_boxes[table_block->box_to_pick];
            printf("S [%d] [%d] [%s]\nBiore pizze: %d, liczba pizz na stole: %d\n", ID, getpid(), timestamp(), pizza_id, table_block->pizzas--);
            table_block->pizza_boxes[table_block->box_to_pick++] = -1;
            table_block->box_to_pick %= TABLE_SIZE;
            unlock(semaphore_id, sops_table);

            sleep(5);
            printf("S [%d] [%d] [%s]\n Dostarczam %d\n", ID, getpid(), timestamp(), pizza_id);

            sleep(5);
        } else {
            unlock(semaphore_id, sops_table);
        }
    }

    return 0;
}




