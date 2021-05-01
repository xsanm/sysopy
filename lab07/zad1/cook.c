#include "shared.h"

int get_random(int _min, int _max) {
    return (rand() %(_max - _min + 1)) + _min;
}


int get_mem_block(char *fname, int proj_id, int size) {
    key_t key = ftok(fname, proj_id);
    if(key == -1) {
        puts("ftok error");
        exit(1);
    }
    return shmget(key, size, 0644);
}

char *add_mem_block(char *fname, int proj_id, int size) {
    int block_id = get_mem_block(fname, proj_id, size);

    if(block_id == -1) {
        return NULL;
    }

    char *block = shmat(block_id, NULL, 0);

    //if ptr goes wrong..
    return block;
}

void remove_mem_block(char *block) {
    if(shmdt(block) == -1) {
        puts("smdt error");
    }
}

char * timestamp(){
    time_t now = time(NULL);
    char * time = asctime(gmtime(&now));
    time[strlen(time)-1] = '\0';
    return time;
}

void diplay_info(int ID) {
    //printf("C [%d] [%d] [%s]\n", ID, getpid(), timestamp());
}


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

    /* klucz, ile semaforow, flagi*/
    int semid;
    if ((semid = semget(key, 2, 0)) == -1) {
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
        int pizza_type = get_random(0, 9);
        int shelf_id;
        diplay_info(ID);
        printf("C [%d] [%d] [%s]\nPrzygotowuje pizze: %d\n", ID, getpid(), timestamp(), pizza_type);
        sleep(2);

        sops_bake->sem_op = -1;
        if (semop(semid, sops_bake, 1) == -1) {
            perror("semop");
            exit(1);
        }

        while(bake_block->pizzas >= BAKE_SIZE) {
            sops_bake->sem_op = 1;
            if (semop(semid, sops_bake, 1) == -1) {
                perror("semop");
                exit(1);
            }
            sleep(1);
            sops_bake->sem_op = -1;
            if (semop(semid, sops_bake, 1) == -1) {
                perror("semop");
                exit(1);
            }
        }

        shelf_id = bake_block->next_shelf++;
        bake_block->next_shelf %= BAKE_SIZE;
        bake_block->bake_shelves[shelf_id] = pizza_type;
        bake_block->pizzas++;

        diplay_info(ID);
        printf("C [%d] [%d] [%s]\nDodalem pizze: %d, liczba pizz w piecu: %d\n", ID, getpid(), timestamp(), pizza_type, bake_block->pizzas);
        sops_bake->sem_op = 1;
        if (semop(semid, sops_bake, 1) == -1) {
            perror("semop");
            exit(1);
        }
        sleep(4);

        sops_bake->sem_op = -1;
        if (semop(semid, sops_bake, 1) == -1) {
            perror("semop");
            exit(1);
        }
        pizza_type = bake_block->bake_shelves[shelf_id];
        bake_block->bake_shelves[shelf_id] = -1;
        bake_block->pizzas--;
        diplay_info(ID);
        printf("C [%d] [%d] [%s]\nWyjalem pizze: %d, liczba pizz w piecu: %d\n", ID, getpid(), timestamp(), pizza_type, bake_block->pizzas);
        sops_bake->sem_op = 1;
        if (semop(semid, sops_bake, 1) == -1) {
            perror("semop");
            exit(1);
        }

        sops_table->sem_op = -1;
        if (semop(semid, sops_table, 1) == -1) {
            perror("semop");
            exit(1);
        }
        table_block->pizza_boxes[table_block->next_box++] = pizza_type;
        table_block->next_box %= TABLE_SIZE;
        table_block->pizzas++;

        diplay_info(ID);
        printf("C [%d] [%d] [%s]\nWYkladam na stol: %d, liczba pizz na stole: %d\n", ID, getpid(), timestamp(), pizza_type, table_block->pizzas);
        sops_table->sem_op = 1;
        if (semop(semid, sops_table, 1) == -1) {
            perror("semop");
            exit(1);
        }

    }


    //remove_mem_block(block);

    return 0;
}