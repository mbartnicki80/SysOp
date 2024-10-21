#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_REINDEER 9
#define NUM_DELIVERIES 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_santa = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_reindeer = PTHREAD_COND_INITIALIZER;

int num_reindeer_waiting = 0;
int num_deliveries = 0;

void *santa_thread(void *arg) {
    while (num_deliveries < NUM_DELIVERIES) {
        pthread_mutex_lock(&mutex);
        while (num_reindeer_waiting < NUM_REINDEER)
            pthread_cond_wait(&cond_santa, &mutex);

        if (num_reindeer_waiting == NUM_REINDEER) {
            printf("Mikołaj: budzę się\n");
            printf("Mikołaj: dostarczam zabawki\n");
            num_deliveries++;
            sleep(rand() % 3 + 2);
            printf("Mikołaj: zasypiam\n");
            num_reindeer_waiting = 0;
            pthread_cond_broadcast(&cond_reindeer);            
        }
        pthread_mutex_unlock(&mutex);
    }
    printf("Mikołaj dostarczył wszystkie zabawki\n");
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_santa);
    pthread_cond_destroy(&cond_reindeer);
    exit(EXIT_SUCCESS);
}

void *reindeer_thread(void *arg) {
    int reindeer_id = *(int *)arg;
    while (num_deliveries < NUM_DELIVERIES) {
        sleep(rand() % 6 + 5);
        
        pthread_mutex_lock(&mutex);
        num_reindeer_waiting++;
        printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n", num_reindeer_waiting, reindeer_id);
        if (num_reindeer_waiting == NUM_REINDEER)
            pthread_cond_signal(&cond_santa);

        pthread_cond_wait(&cond_reindeer, &mutex);
        pthread_mutex_unlock(&mutex);
    }
    printf("Renifer %d kończy pracę\n", reindeer_id);
    pthread_exit(EXIT_SUCCESS);
}

int main() {
    srand(time(NULL));

    pthread_t santa, reindeer[NUM_REINDEER];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_santa, NULL);
    pthread_cond_init(&cond_reindeer, NULL);

    pthread_create(&santa, NULL, santa_thread, NULL);
    int reindeer_ids[NUM_REINDEER];
    for (int i = 0; i < NUM_REINDEER; ++i) {
        reindeer_ids[i] = i;
        pthread_create(&reindeer[i], NULL, reindeer_thread, &reindeer_ids[i]);
    }

    pthread_join(santa, NULL);
    for (int i = 0; i < NUM_REINDEER; ++i)
        pthread_join(reindeer[i], NULL);

    exit(EXIT_SUCCESS);
}