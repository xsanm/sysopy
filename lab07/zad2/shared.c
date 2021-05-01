#include "shared.h"


void lock(int semaphore_id, struct sembuf *sb) {
    sb->sem_op = -1;
    if (semop(semaphore_id, sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void unlock(int semaphore_id, struct sembuf *sb) {
    sb->sem_op = 1;
    if (semop(semaphore_id, sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

char * timestamp() {
    time_t now = time(NULL);
    char * time = asctime(gmtime(&now));
    time[strlen(time)-1] = '\0';
    return time;
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



