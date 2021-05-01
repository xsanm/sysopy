#include "shared.h"


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
    time[strlen(time)-1] = '\0';    // Remove \n
    return time;
}

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

    int semid;
    if ((semid = semget(key, 2, 0)) == -1) {
        perror("semget");
        exit(1);
    }

    struct sembuf *sops_table = malloc(sizeof(struct sembuf));
    sops_table->sem_num = 1;
    sops_table->sem_op = -1;
    sops_table->sem_flg = 0;


    while(1) {
        usleep(rand_time);
        sops_table->sem_op = -1;
        if (semop(semid, sops_table, 1) == -1) {
            perror("semop");
            exit(1);
        }
        if(table_block->pizzas > 0) {
            printf("S [%d] [%d] [%s]\n", ID, getpid(), timestamp());
            printf("biore pizze: %d, liczba pizz na stole: %d\n", table_block->pizza_boxes[table_block->box_to_pick], table_block->pizzas--);
            table_block->pizza_boxes[table_block->box_to_pick++] = -1;
            table_block->box_to_pick %= TABLE_SIZE;
            sops_table->sem_op = 1;
            if (semop(semid, sops_table, 1) == -1) {
                perror("semop");
                exit(1);
            }
            sleep(5);
            printf("S [%d] [%d] [%s]\n", ID, getpid(), timestamp());
            printf("Dostarczam\n");
            sleep(5);
        } else {
            sops_table->sem_op = 1;
            if (semop(semid, sops_table, 1) == -1) {
                perror("semop");
                exit(1);
            }
        }
    }


    //remove_mem_block(block);

    return 0;
}




