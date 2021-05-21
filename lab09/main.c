#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define NO_REINDEERS 9
#define NO_ELVES 10
#define NO_ELVES_WAITING 3

int CNT = 0;

int elves_q[NO_ELVES]; //elves queue
int elves_waiting = 0; //number of elves waiting

int reindeer_waiting = 0; //flag
int reindeer_back = 0; //number of reinders back

pthread_mutex_t mutex_santa = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_reindeer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_reindeer_delivering = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_elf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_elf_solving = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_santa = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_reindeer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_reindeer_delivering = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_elf = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_elf_solving = PTHREAD_COND_INITIALIZER;

void do_work(int a, int b) {
    srand(time(NULL) ^ (pthread_self()<<16));
    int work_time = rand() % (b - a + 1) + a;
    sleep(work_time);
}

void *santa(void *arg) {
    while(1 == 1) {
     pthread_mutex_lock(&mutex_santa);

     while (elves_waiting < NO_ELVES_WAITING && reindeer_back < NO_REINDEERS) {
         printf("Mikolaj: spie, elfy\n");
         pthread_cond_wait(&cond_santa, &mutex_santa);
     }
     pthread_mutex_unlock(&mutex_santa);

     printf("Mikołaj: budze sie\n");

     //reindeers first
     pthread_mutex_lock(&mutex_reindeer);

     if (reindeer_back == NO_REINDEERS) {
         CNT++;
         printf("Mikolaj: dostarczam zabawki [round number: %d]\n", CNT);
         do_work(2, 4);
         reindeer_back = 0;

         // notify waiting reindeer that other reindeer are no longer waiting for santa
         pthread_mutex_lock(&mutex_reindeer_delivering);
         reindeer_waiting = 0;
         pthread_cond_broadcast(&cond_reindeer_delivering);
         pthread_mutex_unlock(&mutex_reindeer_delivering);
     }
     pthread_mutex_unlock(&mutex_reindeer);

     if (CNT == 3) exit(0);

     pthread_mutex_lock(&mutex_elf);
     if (elves_waiting == NO_ELVES_WAITING) {
         printf("Mikolaj: rozwiazuje problemy elfow [%d][%d][%d]\n", elves_q[0], elves_q[1], elves_q[2]);
         do_work(1, 2);


         pthread_mutex_lock(&mutex_elf_solving);
         for (int i = 0; i < NO_ELVES; i++) {
             elves_q[i] = -1;
         }
         elves_waiting = 0;
         pthread_cond_broadcast(&cond_elf_solving);
         pthread_mutex_unlock(&mutex_elf_solving);

     }

     printf("Mikolaj: zasypiam\n\n");
     pthread_mutex_unlock(&mutex_elf);
    }
}

void* reindeer(void* arg) {
    int id = *((int *) arg);
    printf("Reindeer no [%d] alive\n", id);

    while (1 == 1) {
        pthread_mutex_lock(&mutex_reindeer_delivering);
        while (reindeer_waiting == 1) {
            pthread_cond_wait(&cond_reindeer_delivering, &mutex_reindeer_delivering);
        }
        pthread_mutex_unlock(&mutex_reindeer_delivering);

        do_work(5, 10);

        pthread_mutex_lock(&mutex_reindeer);
        reindeer_back++;
        printf("Renifer: czeka [%d] reniferow na Mikolaja, ID [%d]\n", reindeer_back, id);
        reindeer_waiting = 1;

        if (reindeer_back == NO_REINDEERS) {
            printf("Renifer: wybudzam Mikolaja, ID [%d]\n", id);
            pthread_mutex_lock(&mutex_santa);
            pthread_cond_broadcast(&cond_santa);
            pthread_mutex_unlock(&mutex_santa);
        }
        pthread_mutex_unlock(&mutex_reindeer);
    }
}

void* elf(void* arg) {
    int id = *((int *) arg);
    printf("Elf no [%d] alive\n", id);

    while(1 == 1) {
        do_work(2, 5);

        pthread_mutex_lock(&mutex_elf_solving);
        while (elves_waiting == NO_ELVES_WAITING) {
            printf("Elf: czeka na powrot elfow, ID [%d]\n", id);
            pthread_cond_wait(&cond_elf_solving, &mutex_elf_solving); //elfves waiting for 3 to go back
        }
        pthread_mutex_unlock(&mutex_elf_solving);

        pthread_mutex_lock(&mutex_elf);
        if (elves_waiting < NO_ELVES_WAITING){
            elves_q[elves_waiting++] = id;
            printf("Elf: czeka [%d] elfow na Mikolaja, ID [%d]\n", elves_waiting, id);

            if (elves_waiting == NO_ELVES_WAITING){
                printf("Elf: wybudzam Mikolaja, ID [%d]\n", id);
                pthread_mutex_lock(&mutex_santa);
                pthread_cond_broadcast(&cond_santa);
                pthread_mutex_unlock(&mutex_santa);
            }
        }
        pthread_mutex_unlock(&mutex_elf);
    }
}

int main() {
    puts("START");

    for(int i = 0; i < NO_ELVES; i++) {
        elves_q[i] = -1;
    }

    //starting machine
    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, &santa, NULL);

    pthread_t reindeer_thread[NO_REINDEERS];
    for(int i = 0; i < NO_REINDEERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&reindeer_thread[i], NULL, &reindeer, id);
    }

    pthread_t elf_thread[NO_ELVES];
    for(int i = 0; i < NO_ELVES; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&elf_thread[i], NULL, &elf, id);
    }

    //wait for end
    pthread_join(santa_thread, NULL);
    for(int i = 0; i < NO_REINDEERS; i++) {
        pthread_join(reindeer_thread[i], NULL);
    }
    for(int i = 0; i < NO_ELVES; i++) {
        pthread_join(elf_thread[i], NULL);
    }

    return 0;
}