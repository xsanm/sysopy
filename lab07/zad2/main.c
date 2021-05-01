#include "shared.h"

pid_t *workers;
int N, M;

void create_semaphore() {
    sem_t *bake_sem, *table_sem;

    if ((bake_sem = sem_open(BAKE_SEM, O_RDWR | O_CREAT, 0760, 1)) == - 1){
        perror("shm_open");
        exit(1);
    }
    sem_close(bake_sem);
    if ((table_sem = sem_open(TABLE_SEM, O_RDWR | O_CREAT, 0760, 1)) == - 1){
        perror("shm_open");
        exit(1);
    }
    sem_close(table_sem);
}

//void destroy_semaphore(){
//    key_t key;
//    if ((key = ftok(WORKERS_FNAME, WORKERS_ID)) == -1) {
//        perror("ftok");
//        exit(1);
//    }
//
//    int semid;
//    if ((semid = semget(key, 2, 0666 | IPC_CREAT)) == -1) {
//        perror("semget");
//        exit(1);
//    }
//    semctl(semid, 0, IPC_RMID, NULL);
//}


void create_mem_block() {
    int fd_bake, fd_table;
    if ((fd_bake = shm_open(BAKE_FNAME, O_RDWR | O_CREAT, 0760)) == - 1) {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(fd_bake, BLOCK_SIZE) < 0) {
        perror("ftruncate");
        exit(1);
    }
    if ((fd_table = shm_open(TABLE_FNAME, O_RDWR | O_CREAT, 0760)) == - 1) {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(fd_table, BLOCK_SIZE) < 0) {
        perror("ftruncate");
        exit(1);
    }
}

//void destroy_mem_block(char *fname, int proj_id, int size) {
//    int block_id = get_mem_block(fname, proj_id, size);
//    if(block_id == -1) {
//        puts("destoying problem");
//        return;
//    }
//    if(shmctl(block_id, IPC_RMID, NULL) == -1) {
//        puts("shmctl error");
//    }
//}


void run_pizzeria(int cooks_number, int supplier_number) {
    workers = malloc(sizeof (pid_t) * (cooks_number + supplier_number));

    for (int i = 0; i < cooks_number; i++) {
        usleep(rand_time * 2);
        pid_t child_pid = fork();
        workers[i] = child_pid;
        char snum[5];
        sprintf(snum, "%d", i + 1);
        if (child_pid == 0) {
            execlp("./cook",  "./cook", snum, NULL);
        }
    }

    for (int i = 0; i < supplier_number; i++) {
        usleep(rand_time * 2);
        pid_t child_pid = fork();
        workers[cooks_number + i] = child_pid;
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

void sigint_handler(int signum) {
    for (int i = 0; i < N + M; i++) {
        kill(workers[i], SIGINT);
    }
//    destroy_mem_block(BAKE_FNAME, BAKE_ID, BLOCK_SIZE);
//    destroy_mem_block(TABLE_FNAME, TABLE_ID, BLOCK_SIZE);
//    destroy_semaphore();
    exit(0);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    if(argc != 3) {
        puts("WRONG NUMBER OF ARGUMENTS");
        return 1;
    }
    N = strtol (argv[1], NULL, 10);
    M = strtol (argv[2], NULL, 10);
    printf("STARTING PIZZERIA with %d cooks and %d suppliers\n", N, M);

    signal(SIGINT, sigint_handler);

    create_semaphore();
    create_mem_block();

    run_pizzeria(N, M);
//    destroy_mem_block(BAKE_FNAME, BAKE_ID, BLOCK_SIZE);
//    destroy_mem_block(TABLE_FNAME, TABLE_ID, BLOCK_SIZE);
//    destroy_semaphore();

    return 0;
}


