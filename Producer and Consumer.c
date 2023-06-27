#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 10
#define THREADS 2

int buffer[MAX];
int fill = 0;
int use = 0;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1) % MAX;
}

int get() {
    int tmp = buffer[use];
    use = (use + 1) % MAX;
    return tmp;
}

sem_t empty;
sem_t full;

void *producer(void *arg) {
    int i;
    for (i = 0; i < MAX * 2; i++) {
        sem_wait(&empty);
        put(i);
        sem_post(&full);
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < MAX * 2; i++) {
        sem_wait(&full);
        int tmp = get();
        sem_post(&empty);
        printf("%d\n", tmp);
    }
}

int main(int argc, char *argv[]) {
    pthread_t threads[THREADS];
    sem_init(&empty, 0, MAX);
    sem_init(&full, 0, 0);
    pthread_create(&threads[0], NULL, producer, NULL);
    pthread_create(&threads[1], NULL, consumer, NULL);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
}